open Lib;
open Externals;

let videoS3Bucket = "foo";
let videoS3Region = "us-east-1";
let videoMimeType = "video/webm";

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

  let handleFile = (~file, ~coordinates) => {
    let _ =
      createItemMutation(
        ~variables=
          CreateItemMutationConfig.make(
            ~input={
              "file": {
                "bucket": videoS3Bucket,
                "region": videoS3Region,
                "mimeType": file->File._type,
                "localUri": file->Webapi.Url.createObjectURL,
                "key": Externals.UUID.makeV4(),
              },
              "id": Externals.UUID.makeV4(),
            },
            (),
          )##variables,
        (),
      );
    ();
  };

  switch (Constants.browser->Bowser.getBrowserName) {
  | Some(`Safari) =>
    <VideoRecorder.FileInput mimeType=videoMimeType onFile=handleFile />
  | _ =>
    <VideoRecorder.MediaRecorder mimeType=videoMimeType onFile=handleFile />
  };
};
