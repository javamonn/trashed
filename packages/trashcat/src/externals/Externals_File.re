include Webapi.File;

[@bs.deriving abstract]
type blobOptions = {
  [@bs.as "type"]
  type_: string,
};

[@bs.new] external makeBlob: (array(t), blobOptions) => t = "Blob";

external fromString: string => t = "%identity";
external toString: t => string = "%identity";
