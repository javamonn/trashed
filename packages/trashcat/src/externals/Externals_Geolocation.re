[@bs.deriving abstract]
type coordinates =
  pri {
    latitude: float,
    longitude: float,
  };

[@bs.deriving abstract]
type position =
  pri {
    timestamp: int,
    coords: coordinates,
  };

[@bs.deriving abstract]
type positionError =
  pri {
    code: int,
    message: string,
  };

[@bs.deriving abstract]
type currentPositionOptions = {
  [@bs.optional]
  maximumAge: int,
  [@bs.optional]
  timeout: int,
  [@bs.optional]
  enableHighAccuracy: bool,
};

[@bs.val] [@bs.scope ("window", "navigator", "geolocation")]
external getCurrentPosition:
  (position => unit, positionError => unit, currentPositionOptions) => unit =
  "getCurrentPosition";
