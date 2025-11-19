from flask import Flask, request, jsonify, send_from_directory
import subprocess
import json
import os

app = Flask(__name__, static_folder='.')

@app.route('/')
def index():
    return send_from_directory('.', 'index.html')

def call_backend(command, *args):
    """Helper function to call backend API"""
    cmd = ['./backend/backend_api', command] + list(args)
    result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
    
    if result.returncode != 0:
        return {"error": f"Backend error: {result.stderr}", "success": False}
    
    if not result.stdout.strip():
        return {"error": "Backend returned empty", "success": False}
    
    return json.loads(result.stdout)

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
            return jsonify({"success": True, "recipes": []}), 400

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
            return jsonify({"success": True, "recipes": []}), 400

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
            return jsonify({"success": True, "substitutes": []}), 400

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
            return jsonify({"success": True, "recipes": []}), 400

        ingredients_csv = ", ".join(ingredients)
        response = call_backend('search-substitutes', ingredients_csv)
        return jsonify(response)

    except Exception as e:
        return jsonify({"error": str(e), "success": False, "recipes": []}), 500

@app.route('/api/get_recipes', methods=['GET'])
def get_recipes():
    try:
        response = call_backend('list')
        return jsonify(response)
    except Exception as e:
        return jsonify({"error": str(e), "success": False, "recipes": []}), 500

@app.route('/api/statistics', methods=['GET'])
def get_statistics():
    try:
        response = call_backend('stats')
        return jsonify(response)
    except Exception as e:
        return jsonify({"error": str(e), "success": False}), 500

if __name__ == '__main__':
    if not os.path.exists('./backend/backend_api.exe') and not os.path.exists('./backend/backend_api'):
        print("ERROR: backend_api executable not found!")
        exit(1)
    
    print("Starting Recipe Generator Backend...")
    port = int(os.environ.get('PORT', 5000))
    app.run(debug=True, host='0.0.0.0', port=port)
