let geolocation: string = [%raw "require('./geolocation.svg')"];
let trashFavorite: string = [%raw "require('./trash-favorite.svg')"];

[@react.component]
let make = (~className=?, ~icon) => <img ?className src=icon />;
