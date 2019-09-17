open Lib;
open Externals;

module CreateItemMutationConfig = [%graphql
  {|
  mutation CreateItem($input: CreateItemInput!) {
    createItem(input: $input) {
      id
      file {
        bucket
        key
        region
      }
    }
  }
|}
];
module CreateItemMutation =
  ReasonApolloHooks.Mutation.Make(CreateItemMutationConfig);

[@react.component]
let make = () => {
  let (createItemMutation, _s, _f) = CreateItemMutation.use();

  let handleFile = (~file, ~location) => {
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
                "key": "public/" ++ Externals.UUID.makeV4(),
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
    ();
  };

  switch (Constants.browser->Bowser.getBrowserName) {
  | Some(`Safari) =>
    <VideoRecorder.FileInput mimeType="video/webm" onFile=handleFile />
  | _ =>
    <VideoRecorder.MediaRecorder mimeType="video/webm" onFile=handleFile />
  };
};
