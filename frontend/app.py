from flask import Flask, request, jsonify, send_from_directory
import subprocess
import json
import os
import platform

app = Flask(__name__, static_folder='.')

# Detect correct backend executable path
if platform.system() == 'Windows':
    BACKEND_PATH = 'backend\\backend_api.exe'
else:
    BACKEND_PATH = './backend/backend_api'

@app.route('/')
def index():
    return send_from_directory('.', 'index.html')

def call_backend(command, *args):
    """Helper function to call backend API"""
    cmd = [BACKEND_PATH, command] + list(args)
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
        
        if result.returncode != 0:
            print(f"[ERROR] Backend exited with code {result.returncode}")
            print(f"[ERROR] Stderr: {result.stderr}")
            return {"error": f"Backend error: {result.stderr}", "success": False}
        
        if not result.stdout.strip():
            print(f"[ERROR] Backend returned empty output")
            print(f"[ERROR] Stderr: {result.stderr}")
            return {"error": "Backend returned empty", "success": False}
        
        return json.loads(result.stdout)
    
    except subprocess.TimeoutExpired:
        print(f"[ERROR] Backend timeout for command: {command}")
        return {"error": "Backend timeout", "success": False}
    except json.JSONDecodeError as e:
        print(f"[ERROR] Invalid JSON from backend: {e}")
        print(f"[DEBUG] Raw output: {result.stdout[:200]}")
        return {"error": f"Invalid JSON: {e}", "success": False}
    except Exception as e:
        print(f"[ERROR] Exception: {e}")
        return {"error": str(e), "success": False}

@app.route('/api/add_recipe', methods=['POST'])
def add_recipe():
    try:
        data = request.json
        name = data.get('name', '').strip()
        ingredients = data.get('ingredients', [])
        instructions = data.get('instructions', '').strip()

        if not name or not ingredients or not instructions:
            return jsonify({"error": "Missing fields", "success": False}), 400

        ingredients_csv = ", ".join(ingredients)
        response = call_backend('add', name, ingredients_csv, instructions)
        return jsonify(response)

    except Exception as e:
        return jsonify({"error": str(e), "success": False}), 500

@app.route('/api/search_recipes', methods=['POST'])
def search_recipes():
    try:
        data = request.json
        ingredients = data.get('ingredients', [])

        if not ingredients:
            return jsonify({"success": True, "recipes": []}), 200

        ingredients_csv = ", ".join(ingredients)
        response = call_backend('search', ingredients_csv)
        return jsonify(response)

    except Exception as e:
        return jsonify({"error": str(e), "success": False, "recipes": []}), 500

@app.route('/api/search_ranked', methods=['POST'])
def search_ranked():
    try:
        data = request.json
        ingredients = data.get('ingredients', [])

        if not ingredients:
            return jsonify({"success": True, "recipes": []}), 200

        ingredients_csv = ", ".join(ingredients)
        response = call_backend('search-ranked', ingredients_csv)
        return jsonify(response)

    except Exception as e:
        return jsonify({"error": str(e), "success": False, "recipes": []}), 500

@app.route('/api/get_substitutes', methods=['POST'])
def get_substitutes():
    try:
        data = request.json
        ingredient = data.get('ingredient', '').strip()

        if not ingredient:
            return jsonify({"success": True, "substitutes": []}), 200

        response = call_backend('get-substitutes', ingredient)
        return jsonify(response)

    except Exception as e:
        return jsonify({"error": str(e), "success": False, "substitutes": []}), 500

@app.route('/api/search_with_substitutes', methods=['POST'])
def search_with_substitutes():
    try:
        data = request.json
        ingredients = data.get('ingredients', [])

        if not ingredients:
            return jsonify({"success": True, "recipes": []}), 200

        ingredients_csv = ", ".join(ingredients)
        response = call_backend('search-substitutes', ingredients_csv)
        return jsonify(response)

    except Exception as e:
        return jsonify({"error": str(e), "success": False, "recipes": []}), 500

@app.route('/api/get_recipes', methods=['GET'])
def get_recipes():
    try:
        print("[INFO] Calling backend: list")
        response = call_backend('list')
        print(f"[INFO] Backend response: {response}")
        return jsonify(response)
    except Exception as e:
        print(f"[ERROR] Exception in get_recipes: {e}")
        return jsonify({"error": str(e), "success": False, "recipes": []}), 500

@app.route('/api/statistics', methods=['GET'])
def get_statistics():
    try:
        response = call_backend('stats')
        return jsonify(response)
    except Exception as e:
        return jsonify({"error": str(e), "success": False}), 500

if __name__ == '__main__':
    # Check if backend executable exists
    if not os.path.exists(BACKEND_PATH):
        print(f"ERROR: Backend executable not found at: {BACKEND_PATH}")
        print("Please compile the backend first:")
        print("  gcc -o backend/backend_api backend_api.c recipe_manager.c ingredient_map.c recipe_ranking.c ingredient_substitution.c file_storage.c -lm")
        exit(1)
    
    print(f"Backend executable found: {BACKEND_PATH}")
    print("Starting Recipe Generator Backend...")
    print("Server: http://localhost:5000")
    
    port = int(os.environ.get('PORT', 5000))
    app.run(debug=True, host='0.0.0.0', port=port)
