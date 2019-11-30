let geolocation: string = [%raw "require('./geolocation.svg')"];

[@react.component]
let make = (~className=?, ~icon) => <img ?className src=icon />;
