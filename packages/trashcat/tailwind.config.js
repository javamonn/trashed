const c = require('chroma-js');

module.exports = {
  theme: {
    extend: {
      colors: {
        primary: c('#000')
          .alpha(0.87)
          .css(),
        brandPrimary: c('#93B5C6').css()
      },
    },
  },
};
