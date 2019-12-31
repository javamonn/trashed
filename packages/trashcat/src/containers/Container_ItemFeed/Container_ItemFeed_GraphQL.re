module Config = [%graphql
  {|
    query NearbyItems($location: LocationInput, $m: Int, $nextToken: String)  {
      nearbyItems(location: $location, m: $m, nextToken: $nextToken, limit: 30) {
        nextToken
        items {
          id
          ...Container_Item.GetItemFragment.ItemFragment @bsField(name: "itemFragment")
        }
      }
    }
  |}
];

module Json = {
  type dict = Js.Dict.t(string);

  [@decco]
  type nearbyItems = {
    nextToken: option(string),
    items: option(array([@decco.codec Decco.Codecs.magic] dict)),
    [@decco.key "__typename"]
    typename: option(string),
  };

  [@decco]
  type t = {nearbyItems: option(nearbyItems)};

  let decode = t_decode;
  let encode = t_encode;
};

module Variables = {

  [@decco]
  type location = {
    lat: float,
    lon: float,
  };

  [@decco]
  type t = {
    location: option(location),
    m: option(int),
    nextToken: option(string),
  };

  let decode = json => json->t_decode->Belt.Result.getExn;
  let encode = t_encode;

  let make = (~location=?, ~m=?, ~nextToken=?, ()) => { location, m, nextToken }
};
