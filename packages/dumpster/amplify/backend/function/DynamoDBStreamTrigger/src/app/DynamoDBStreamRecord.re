module Field = {
  type list('a) = {. "L": array('a)};
  type string = {. "S": string};
  type null = {. "N": bool};
  type nullableString = {
    .
    "N": bool,
    "S": string,
  };
};

module VideoRecord = {
  type attributeMap = {
    .
    "files":
      Field.list({
        .
        "cloudfrontUrl": Field.nullableString,
        "mimeType": Field.string,
        "file": {
          .
          "bucket": Field.string,
          "key": Field.string,
          "region": Field.string,
        },
      }),
    "id": Field.string,
    "createdAt": Field.string,
    "updatedAt": Field.string,
    "videoMediaConvertJobId": Field.nullableString,
    "thumbnail": Field.null,
  };

  [@bs.deriving abstract]
  type t =
    pri {
      [@bs.as "ApproximateCreationTimeDate"]
      approximateCreationTimeDate: string,
      [@bs.as "NewImage"]
      newImage: attributeMap,
      [@bs.as "SequenceNumber"]
      sequenceNumber: string,
      [@bs.as "SizeBytes"]
      sizeBytes: int,
      [@bs.as "StreamViewType"]
      streamViewType: string,
    };
};

[@bs.deriving abstract]
type t =
  pri {
    eventName: string,
    eventSource: string,
    eventId: string,
    eventVersion: string,
    awsRegion: string,
    eventSourceARN: string,
    dynamodb: VideoRecord.t,
  };

/** FIXME: Table name changes across env */
[@bs.deriving jsConverter]
type tableName = [
  | [@bs.as "Video-akkeqcns2fc5hn7ad3uu3euymy-production"] `VideoTable
];

let tableGet = inst =>
  (inst->eventSourceARNGet |> Js.String.split("/"))
  ->Belt.Array.get(1)
  ->Belt.Option.flatMap(tableNameFromJs);
