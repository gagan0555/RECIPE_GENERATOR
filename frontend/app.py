from flask import Flask, request, jsonify, send_from_directory
import subprocess
import json
import os
import sys

app = Flask(__name__, static_folder='.')

# Serve the frontend HTML
@app.route('/')
def index():
    print("[INFO] Serving index.html")
    return send_from_directory('.', 'index.html')

# API: Add Recipe
@app.route('/api/add_recipe', methods=['POST'])
def add_recipe():
    try:
        print("\n[API] POST /api/add_recipe called")
        data = request.json
        print(f"[INFO] Request data: {data}")

        name = data.get('name', '').strip()
        ingredients = data.get('ingredients', [])
        instructions = data.get('instructions', '').strip()

        print(f"[DEBUG] Name: {name}, Ingredients: {ingredients}, Instructions: {instructions[:50]}...")

        if not name or not ingredients or not instructions:
            error_msg = "Missing required fields"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False}), 400

        ingredients_csv = ", ".join(ingredients)
        print(f"[DEBUG] Calling backend_api with: add '{name}' '{ingredients_csv}' '{instructions}'")

        result = subprocess.run(
            ['./backend_api', 'add', name, ingredients_csv, instructions],
            capture_output=True,
            text=True,
            timeout=5
        )

        print(f"[DEBUG] Return code: {result.returncode}")
        print(f"[DEBUG] Stdout: {result.stdout}")
        if result.stderr:
            print(f"[DEBUG] Stderr: {result.stderr}")

        if result.returncode != 0:
            error_msg = f"Backend error (exit code {result.returncode}): {result.stderr}"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False}), 500

        if not result.stdout.strip():
            error_msg = "Backend returned empty response"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False}), 500

        response = json.loads(result.stdout)
        print(f"[INFO] Success: {response}")
        return jsonify(response)

    except subprocess.TimeoutExpired:
        error_msg = "Backend timeout (5 seconds)"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False}), 500
    except json.JSONDecodeError as e:
        error_msg = f"Invalid JSON from backend: {e}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False}), 500
    except Exception as e:
        error_msg = f"Exception: {str(e)}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False}), 500

# API: Search Recipes (basic)
@app.route('/api/search_recipes', methods=['POST'])
def search_recipes():
    try:
        print("\n[API] POST /api/search_recipes called")
        data = request.json
        ingredients = data.get('ingredients', [])
        print(f"[INFO] Search ingredients: {ingredients}")

        if not ingredients:
            return jsonify({"error": "Missing ingredients", "success": False, "recipes": []}), 400

        ingredients_csv = ", ".join(ingredients)
        print(f"[DEBUG] Calling backend_api with: search '{ingredients_csv}'")

        result = subprocess.run(
            ['./backend_api', 'search', ingredients_csv],
            capture_output=True,
            text=True,
            timeout=5
        )

        print(f"[DEBUG] Return code: {result.returncode}")
        print(f"[DEBUG] Stdout: {result.stdout[:200]}...")
        if result.stderr:
            print(f"[DEBUG] Stderr: {result.stderr}")

        if result.returncode != 0:
            error_msg = f"Backend error: {result.stderr}"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

        if not result.stdout.strip():
            error_msg = "Backend returned empty response"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

        response = json.loads(result.stdout)
        print(f"[INFO] Found {len(response.get('recipes', []))} recipes")
        return jsonify(response)

    except subprocess.TimeoutExpired:
        error_msg = "Backend timeout"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500
    except json.JSONDecodeError as e:
        error_msg = f"Invalid JSON from backend: {e}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500
    except Exception as e:
        error_msg = f"Exception: {str(e)}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

# NEW API: Search Recipes with Ranking
@app.route('/api/search_ranked', methods=['POST'])
def search_ranked():
    try:
        print("\n[API] POST /api/search_ranked called")
        data = request.json
        ingredients = data.get('ingredients', [])
        print(f"[INFO] Search ingredients (ranked): {ingredients}")

        if not ingredients:
            return jsonify({"error": "Missing ingredients", "success": False, "recipes": []}), 400

        ingredients_csv = ", ".join(ingredients)
        print(f"[DEBUG] Calling backend_api with: search_ranked '{ingredients_csv}'")

        result = subprocess.run(
            ['./backend_api', 'search_ranked', ingredients_csv],
            capture_output=True,
            text=True,
            timeout=5
        )

        print(f"[DEBUG] Return code: {result.returncode}")
        print(f"[DEBUG] Stdout: {result.stdout[:200]}...")
        if result.stderr:
            print(f"[DEBUG] Stderr: {result.stderr}")

        if result.returncode != 0:
            error_msg = f"Backend error: {result.stderr}"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

        if not result.stdout.strip():
            error_msg = "Backend returned empty response"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

        response = json.loads(result.stdout)
        print(f"[INFO] Found {len(response.get('recipes', []))} ranked recipes")
        return jsonify(response)

    except subprocess.TimeoutExpired:
        error_msg = "Backend timeout"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500
    except json.JSONDecodeError as e:
        error_msg = f"Invalid JSON from backend: {e}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500
    except Exception as e:
        error_msg = f"Exception: {str(e)}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

# NEW API: Get Ingredient Substitutes
@app.route('/api/get_substitutes', methods=['POST'])
def get_substitutes():
    try:
        print("\n[API] POST /api/get_substitutes called")
        data = request.json
        ingredient = data.get('ingredient', '').strip()
        print(f"[INFO] Get substitutes for: {ingredient}")

        if not ingredient:
            return jsonify({"error": "Missing ingredient", "success": False, "substitutes": []}), 400

        print(f"[DEBUG] Calling backend_api with: substitutes '{ingredient}'")

        result = subprocess.run(
            ['./backend_api', 'substitutes', ingredient],
            capture_output=True,
            text=True,
            timeout=5
        )

        print(f"[DEBUG] Return code: {result.returncode}")
        print(f"[DEBUG] Stdout: {result.stdout}")
        if result.stderr:
            print(f"[DEBUG] Stderr: {result.stderr}")

        if result.returncode != 0:
            error_msg = f"Backend error: {result.stderr}"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False, "substitutes": []}), 500

        if not result.stdout.strip():
            error_msg = "Backend returned empty response"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False, "substitutes": []}), 500

        response = json.loads(result.stdout)
        print(f"[INFO] Found {len(response.get('substitutes', []))} substitutes")
        return jsonify(response)

    except subprocess.TimeoutExpired:
        error_msg = "Backend timeout"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "substitutes": []}), 500
    except json.JSONDecodeError as e:
        error_msg = f"Invalid JSON from backend: {e}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "substitutes": []}), 500
    except Exception as e:
        error_msg = f"Exception: {str(e)}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "substitutes": []}), 500

# NEW API: Search with Substitution Support
@app.route('/api/search_with_substitutes', methods=['POST'])
def search_with_substitutes():
    try:
        print("\n[API] POST /api/search_with_substitutes called")
        data = request.json
        ingredients = data.get('ingredients', [])
        print(f"[INFO] Search with substitutes for: {ingredients}")

        if not ingredients:
            return jsonify({"error": "Missing ingredients", "success": False, "recipes": []}), 400

        ingredients_csv = ", ".join(ingredients)
        print(f"[DEBUG] Calling backend_api with: search_with_subs '{ingredients_csv}'")

        result = subprocess.run(
            ['./backend_api', 'search_with_subs', ingredients_csv],
            capture_output=True,
            text=True,
            timeout=5
        )

        print(f"[DEBUG] Return code: {result.returncode}")
        print(f"[DEBUG] Stdout: {result.stdout[:200]}...")
        if result.stderr:
            print(f"[DEBUG] Stderr: {result.stderr}")

        if result.returncode != 0:
            error_msg = f"Backend error: {result.stderr}"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

        if not result.stdout.strip():
            error_msg = "Backend returned empty response"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

        response = json.loads(result.stdout)
        print(f"[INFO] Found {len(response.get('recipes', []))} recipes with substitutes")
        return jsonify(response)

    except subprocess.TimeoutExpired:
        error_msg = "Backend timeout"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500
    except json.JSONDecodeError as e:
        error_msg = f"Invalid JSON from backend: {e}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500
    except Exception as e:
        error_msg = f"Exception: {str(e)}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

# API: Get All Recipes
@app.route('/api/get_recipes', methods=['GET'])
def get_recipes():
    try:
        print("\n[API] GET /api/get_recipes called")
        print(f"[DEBUG] Calling backend_api with: list")

        result = subprocess.run(
            ['./backend_api', 'list'],
            capture_output=True,
            text=True,
            timeout=5
        )

        print(f"[DEBUG] Return code: {result.returncode}")
        print(f"[DEBUG] Stdout length: {len(result.stdout)}")
        print(f"[DEBUG] Stdout (first 300 chars): {result.stdout[:300]}...")
        if result.stderr:
            print(f"[DEBUG] Stderr: {result.stderr}")

        if result.returncode != 0:
            error_msg = f"Backend error (exit code {result.returncode}): {result.stderr}"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

        if not result.stdout.strip():
            error_msg = "Backend returned empty response"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

        response = json.loads(result.stdout)
        recipe_count = len(response.get('recipes', []))
        print(f"[INFO] Found {recipe_count} recipes")
        return jsonify(response)

    except subprocess.TimeoutExpired:
        error_msg = "Backend timeout (5 seconds)"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500
    except json.JSONDecodeError as e:
        error_msg = f"Invalid JSON from backend: {e}"
        print(f"[ERROR] {error_msg}")
        print(f"[DEBUG] Raw output was: {result.stdout}")
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500
    except Exception as e:
        error_msg = f"Exception: {str(e)}"
        print(f"[ERROR] {error_msg}")
        import traceback
        traceback.print_exc()
        return jsonify({"error": error_msg, "success": False, "recipes": []}), 500

# API: Get Statistics
@app.route('/api/statistics', methods=['GET'])
def get_statistics():
    try:
        print("\n[API] GET /api/statistics called")
        print(f"[DEBUG] Calling backend_api with: stats")

        result = subprocess.run(
            ['./backend_api', 'stats'],
            capture_output=True,
            text=True,
            timeout=5
        )

        print(f"[DEBUG] Return code: {result.returncode}")
        print(f"[DEBUG] Stdout: {result.stdout}")
        if result.stderr:
            print(f"[DEBUG] Stderr: {result.stderr}")

        if result.returncode != 0:
            error_msg = f"Backend error: {result.stderr}"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False}), 500

        if not result.stdout.strip():
            error_msg = "Backend returned empty response"
            print(f"[ERROR] {error_msg}")
            return jsonify({"error": error_msg, "success": False}), 500

        response = json.loads(result.stdout)
        print(f"[INFO] Stats: {response}")
        return jsonify(response)

    except subprocess.TimeoutExpired:
        error_msg = "Backend timeout"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False}), 500
    except json.JSONDecodeError as e:
        error_msg = f"Invalid JSON from backend: {e}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False}), 500
    except Exception as e:
        error_msg = f"Exception: {str(e)}"
        print(f"[ERROR] {error_msg}")
        return jsonify({"error": error_msg, "success": False}), 500

if __name__ == '__main__':
    print("\n" + "="*60)
    print("🍳 Recipe Generator Backend Server (ENHANCED)")
    print("="*60)
    print(f"Starting on http://0.0.0.0:5000")
    print(f"Frontend: http://localhost:5000")
    print(f"Make sure 'backend_api' executable is in current directory!")
    print("\nNEW ENDPOINTS:")
    print("  - POST /api/search_ranked")
    print("  - POST /api/get_substitutes")
    print("  - POST /api/search_with_substitutes")
    print("="*60 + "\n")

    port = int(os.environ.get('PORT', 5000))
    app.run(debug=True, host='0.0.0.0', port=port)
