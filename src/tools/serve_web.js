// Static server para la build web de Cadash Remake.
// Uso: node serve_web.js [carpeta] [puerto]
//   - carpeta por defecto: ../../web  (relativo a src/tools)
//   - puerto por defecto: 8080
// Necesario porque "python -m http.server" NO sirve .wasm con el MIME
// correcto (application/wasm) y Chrome lo rechaza con pantalla en blanco.
// Configura MIME para .wasm/.js/.html y Cache-Control: no-store.

const http = require('http');
const fs = require('fs');
const path = require('path');

const root = process.argv[2] || path.resolve(__dirname, '../../web');
const port = Number(process.argv[3] || 8080);

const types = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.mjs': 'text/javascript; charset=utf-8',
  '.wasm': 'application/wasm',
  '.png': 'image/png',
  '.json': 'application/json; charset=utf-8',
  '.css': 'text/css; charset=utf-8',
};

http.createServer((req, res) => {
  const urlPath = decodeURIComponent((req.url || '/').split('?')[0]);
  let file = path.normalize(path.join(root, urlPath === '/' ? 'index.html' : urlPath));
  if (!file.startsWith(path.normalize(root))) {
    res.writeHead(403);
    res.end('forbidden');
    return;
  }
  if (!fs.existsSync(file) || fs.statSync(file).isDirectory()) {
    res.writeHead(404);
    res.end('not found');
    return;
  }
  const ext = path.extname(file).toLowerCase();
  res.writeHead(200, {
    'Content-Type': (types[ext] || 'application/octet-stream'),
    'Cache-Control': 'no-store, no-cache, must-revalidate',
  });
  fs.createReadStream(file).pipe(res);
}).listen(port, '0.0.0.0', () => {
  console.log('Sirviendo ' + root + ' en http://localhost:' + port + '  (Ctrl+F5 para recargar sin cache)');
});
