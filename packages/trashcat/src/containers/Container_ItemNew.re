open Lib;
open Externals;

module CreateItemVideoMutation = [%graphql
  {|
  mutation CreateItemVideo($input: CreateVideoInput!) {
    createVideo(input: $input) {
      id
      files {
        file {
          bucket
          key
          region
        }
        mimeType
      }
    }
  }
|}
];

module CreateItemMutation = [%graphql
  {|
  mutation CreateItem($input: CreateItemInput!) {
    createItem(input: $input) {
      id
      video {
        id
        files {
          file {
            bucket
            key
            region
          }
          mimeType
        }
      }
      location {
        lat
        lon
      }
    }
  }
|}
];

[@react.component]
let make = (~isActive) => {
  let (createItemVideoMutation, _s, _f) =
    ApolloHooks.useMutation(CreateItemVideoMutation.definition);
  let (createItemMutation, _s, _f) =
    ApolloHooks.useMutation(CreateItemMutation.definition);

  let (notificationIn, setNotificationIn) = React.useState(() => false);
  let timeoutIdRef = React.useRef(None);

  let clearTimeout = () =>
    switch (timeoutIdRef->React.Ref.current) {
    | Some(id) =>
      let _ = id->Js.Global.clearTimeout;
      let _ = timeoutIdRef->React.Ref.setCurrent(None);
      ();
    | None => ()
    };

  let handleDisplayNotification = () => {
    let _ = setNotificationIn(_ => true);
    let _ = clearTimeout();
    let timeoutId =
      Js.Global.setTimeout(
        () => {
          let _ = setNotificationIn(_ => false);
          let _ = clearTimeout();
          ();
        },
        4000,
      );
    let _ = timeoutIdRef->React.Ref.setCurrent(Some(timeoutId));
    ();
  };

  let _ = React.useEffect0(() => Some(() => clearTimeout()));

  let handleFile = (~file, ~location) => {
    let _ =
      createItemVideoMutation(
        ~variables=
          CreateItemVideoMutation.makeVariables(
            ~input={
              "id": Externals.UUID.makeV4()->Js.Option.some,
              "files": [|
                {
                  "mimeType":
                    file
                    ->File._type
                    ->VideoSurface.MimeType.fromJs
                    ->Belt.Option.getWithDefault(`WEBM),
                  "file": {
                    "bucket":
                      AwsAmplify.(
                        Constants.awsAmplifyConfig->Config.userFilesS3BucketGet
                      ),
                    "region":
                      AwsAmplify.(
                        Constants.awsAmplifyConfig->Config.userFilesS3BucketRegionGet
                      ),
                    "mimeType": file->File._type->Js.Option.some,
                    "localUri": file->File.toString->Js.Option.some,
                    "key":
                      "public/item-video-upload/" ++ Externals.UUID.makeV4(),
                  },
                  "userUpload": true,
                },
              |],
              "poster": None,
              "thumbnail": None,
              "videoMediaConvertJobId": None,
            },
            (),
          ),
        (),
      )
      |> Js.Promise.then_(r =>
           switch (r) {
           | ApolloHooks.Mutation.Data(data) =>
             data##createVideo
             ->Belt.Option.map(video =>
                 createItemMutation(
                   ~variables=
                     CreateItemMutation.makeVariables(
                       ~input={
                         "itemVideoId": video##id,
                         "location": {
                           "lat": location->Geolocation.latitudeGet,
                           "lon": location->Geolocation.longitudeGet,
                         },
                         "id": Externals.UUID.makeV4()->Js.Option.some,
                       },
                       (),
                     ),
                   (),
                 )
                 |> Js.Promise.then_(_r => Js.Promise.resolve())
               )
             ->Belt.Option.getWithDefault(Js.Promise.resolve())
           | Error(_error) => Js.Promise.resolve()
           | _ => Js.Promise.resolve()
           }
         );
    let _ = handleDisplayNotification();
    ();
  };

  <>
    <Notification.ItemPosted _in=notificationIn />
    {switch (Constants.browser->Bowser.getBrowserName) {
     | Some(`Safari) => <VideoRecorder.FileInput onFile=handleFile />
     | _ =>
       <VideoRecorder.MediaRecorder
         mimeType=`WEBM
         onFile=handleFile
         isActive
       />
     }}
  </>;
};
