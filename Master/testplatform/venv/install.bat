python install.py
cd %~dp0..
git config --global user.name "%username%"
git config --global user.email "%username%@casco"
git init
git add -A
git commit -m "init"
pause