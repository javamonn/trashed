type t;

module Track = {
  type t;
  [@bs.send] external stop: t => unit = "stop";
};

[@bs.send] external getTracks: t => array(Track.t) = "getTracks";
