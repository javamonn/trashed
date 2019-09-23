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
let make = () => {
  let (createItemVideoMutation, _s, _f) = CreateItemVideoMutation.use();
  let (createItemMutation, _s, _f) = CreateItemMutation.use();

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
                      Amplify.(config->Config.s3ItemVideoUploadBucketGet),
                    "region":
                      Amplify.(
                        config->Config.s3ItemVideoUploadBucketRegionGet
                      ),
                    "mimeType": file->File._type->Js.Option.some,
                    "localUri": file->File.toString->Js.Option.some,
                    "key":
                      "public/item-video-upload" ++ Externals.UUID.makeV4(),
                  },
                  "cloudfrontUrl": None,
                },
              |],
              "thumbnail": None,
              "videoMediaConvertJobId": None,
            },
            (),
          )##variables,
        (),
      );

    /**
    let _ =
      createItemMutation(
        ~variables=
          CreateItemMutationConfig.make(
            ~input={
              "file": {
                "bucket": Amplify.(config->Config.s3ItemVideoUploadBucketGet),
                "region":
                  Amplify.(config->Config.s3ItemVideoUploadBucketRegionGet),
                "mimeType": file->File._type->Js.Option.some,
                "localUri": file->File.toString->Js.Option.some,
                "key": "public/item-video-uploads" ++ Externals.UUID.makeV4(),
              },
              "location": {
                "lat": location->Geolocation.latitudeGet,
                "lon": location->Geolocation.longitudeGet,
              },
              "id": Externals.UUID.makeV4()->Js.Option.some,
            },
            (),
          )##variables,
        (),
      );
    **/
    ();
  };

  switch (Constants.browser->Bowser.getBrowserName) {
  | Some(`Safari) =>
    <VideoRecorder.FileInput mimeType=`WEBM onFile=handleFile />
  | _ => <VideoRecorder.MediaRecorder mimeType=`WEBM onFile=handleFile />
  };
};
