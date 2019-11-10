const static = require('node-static');
const fileServer = new static.Server('./dist');

require('http')
  .createServer((request, response) => {
    request
      .addListener('end', () => {
        if (
          request.url.includes('.js') ||
          request.url.includes('.pcss') ||
          request.url.includes('.mjs')
        ) {
          const url = request.url.split('?')[0];
          fileServer.serveFile(
            url,
            200,
            {'Content-Type': 'application/javascript'},
            request,
            response
          )
            .addListener('error', (err) => { 
              console.error(err) 
              response.writeHead(500)
              response.end()
            });

        } else if (
          !request.url.includes('/build/') &&
          !request.url.includes('/images')
        ) {
          const url = 'index.html';
          fileServer.serveFile(url, 200, {}, request, response);
        } else {
          fileServer.serve(request, response);
        }
      })
      .addListener('error', () => {
        response.writeHead(500);
        response.end()
      })
      .resume();
  })
  .listen(8080);
