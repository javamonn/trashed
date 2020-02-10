let _ = Amplitude.(getInstance()->init(Lib_Constants.Env.amplitudeApiKey));

[@decco.encode]
type graphqlOperation = {
  variables: Js.Json.t,
  operationName: string,
};

[@decco.encode]
type page = {path: string};

[@bs.deriving accessors]
type event =
  | GraphQLOperation(graphqlOperation)
  | Page(page);

let track = event => {
  let (name, properties) =
    switch (event) {
    | GraphQLOperation(p) => (
        "GRAPH_QL_OPERATION",
        graphqlOperation_encode(p),
      )
    | Page(p) => ("PAGE", page_encode(p))
    };

  Amplitude.(getInstance()->logEventWithProperties(name, properties));
};
