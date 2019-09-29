module ArrayField = {
  [@bs.deriving abstract]
  type t('a) =
    pri {
      [@bs.as "L"]
      array: array('a),
    };

  let get = arrayGet;
};

module StringField = {
  [@bs.deriving abstract]
  type t =
    pri {
      [@bs.as "S"]
      string,
    };
  let get = stringGet;
};

module NullField = {
  [@bs.deriving abstract]
  type t =
    pri {
      [@bs.as "N"]
      null: bool,
    };
  let get = nullGet;
};

module NullableStringField = {
  [@bs.deriving abstract]
  type t =
    pri {
      [@bs.as "N"]
      null: bool,
      [@bs.as "S"]
      string,
    };

  let get = i => nullGet(i) ? None : Some(stringGet(i));
};

module VideoRecord = {
  type attributeMap = {
    .
    "files":
      ArrayField.t({
        .
        "cloudfrontUrl": NullableStringField.t,
        "mimeType": StringField.t,
        "file": {
          .
          "bucket": StringField.t,
          "key": StringField.t,
          "region": StringField.t,
        },
      }),
    "id": StringField.t,
    "createdAt": StringField.t,
    "updatedAt": StringField.t,
    "videoMediaConvertJobId": NullableStringField.t,
    "thumbnail": NullField.t,
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

[@bs.deriving jsConverter]
type tableName = [ | [@bs.as "Video"] `VideoTable];

let tableGet = inst =>
  (inst->eventSourceARNGet |> Js.String.split("/"))
  ->Belt.Array.get(1)
  ->Belt.Option.map(Js.String.split("-"))
  ->Belt.Option.flatMap(a => Belt.Array.get(a, 0))
  ->Belt.Option.flatMap(tableNameFromJs);
