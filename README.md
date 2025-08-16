# Office On Fire

### Dependency
- [emsdk](https://emscripten.org/docs/getting_started/downloads.html)

### Local development
In `index.html`, add Supabase URL and KEY.
```shell
./build.sh # compile C++ to JS
python3 -m http.server 8080
```

