open Lib;
open Externals;

module CreateItemVideoMutationConfig = [%graphql
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
module CreateItemVideoMutation =
  ReasonApolloHooks.Mutation.Make(CreateItemVideoMutationConfig);

module CreateItemMutationConfig = [%graphql
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
module CreateItemMutation =
  ReasonApolloHooks.Mutation.Make(CreateItemMutationConfig);

[@react.component]
let make = (~isActive) => {
  let (createItemVideoMutation, _s, _f) = CreateItemVideoMutation.use();
  let (createItemMutation, _s, _f) = CreateItemMutation.use();

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
          CreateItemVideoMutationConfig.make(
            ~input={
              "id": Externals.UUID.makeV4()->Js.Option.some,
              "files": [|
                {
                  "mimeType":
                    file
                    ->File._type
                    ->VideoSurface.mimeTypeFromJs
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
          )##variables,
        (),
      )
      |> Js.Promise.then_(r =>
           switch (r) {
           | ReasonApolloHooks.Mutation.Data(data) =>
             data##createVideo
             ->Belt.Option.map(video =>
                 createItemMutation(
                   ~variables=
                     CreateItemMutationConfig.make(
                       ~input={
                         "itemVideoId": video##id,
                         "location": {
                           "lat": location->Geolocation.latitudeGet,
                           "lon": location->Geolocation.longitudeGet,
                         },
                         "id": Externals.UUID.makeV4()->Js.Option.some,
                       },
                       (),
                     )##variables,
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
     | Some(`Safari) =>
       <VideoRecorder.FileInput mimeType=`WEBM onFile=handleFile />
     | _ =>
       <VideoRecorder.MediaRecorder
         mimeType=`WEBM
         onFile=handleFile
         isActive
       />
     }}
  </>;
};
