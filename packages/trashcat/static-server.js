const static = require('node-static');
const fileServer = new static.Server('./dist');

require('http')
  .createServer((request, response) => {
    request
      .addListener('end', () => {
        if (request.url.includes('.js') || request.url.includes('.pcss')) {
          fileServer.serveFile(
            request.url,
            200,
            {'Content-Type': 'application/javascript'},
            request,
            response,
          );
        } else {
          fileServer.serve(request, response);
        }
      })
      .resume();
  })
  .listen(8080);
