[@react.component]
let make = (~className, ~backgroundColor) => {
  <svg
    className
    viewBox="0 0 192 192"
    fill="none"
    xmlns="http://www.w3.org/2000/svg">
    <rect x="6" y="6" width="186" height="186" fill="black" />
    <rect width="186" height="186" fill=backgroundColor />
  </svg>;
};
