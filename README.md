*Recipe Manager Project*


*Welcome to the Recipe Manager project! This repository contains the source code for a full-stack recipe management system with a C backend and a Python Flask frontend.



*Project Description
~This project implements a recipe manager with the following features:
~Backend implemented in C for fast data processing
~Frontend powered by Python Flask to interact with users via API and some UI
~Persistent storage of recipes in binary files
~Features include recipe addition, search by ingredients, listing, and statistics
~Ready for cloud deployment with clear build and deployment instructions



*Project Structure
text
backend/
│  backend_api.c
│  recipe_manager.c
│  file_storage.c
│  recipe_ranking.c
│  ingredient_substitution.c
│  ingredient_map_fixed.c and related headers
│  Makefile
│  backend_api (compiled executable)
frontend/
│  app_enhanced.py (Flask app)
│  index.html (Frontend UI)
│  requirements.txt (Python dependencies)
README.md


*Setup Instructions

-Building Backend
Run make in the backend/ folder to build backend_api
Requires gcc with C99 support

-Running Frontend
Use Python 3 and install dependencies:
pip install -r frontend/requirements.txt
Run Flask app:
python frontend/app.py
Access at http://localhost:5000

*Contributing
Feel free to fork and create pull requests. Please adhere to code style and include tests if possible

*Contact
For queries or support, contact [gagneeshsingh@gmail.com].
