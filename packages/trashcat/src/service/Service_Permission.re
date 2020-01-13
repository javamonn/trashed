module Permissions = Externals_Permissions;
module MediaStream = Externals_MediaStream;
module MediaDevices = Externals_MediaDevices;
module LocalStorage = Externals_LocalStorage;

[@bs.deriving accessors]
type status('data) =
  | Unknown
  | Unprompted
  | TransitionInProgress(Js.Promise.t(status('data)))
  | PermissionGranted(option('data))
  | PermissionRejected;

let permissionStatusChangeEvent = "trashcat-permissionStatusChange";
module Storage = {
  let localStorageNamespace = "trashcat-servicePermission-";
  let getKey = permissionName =>
    localStorageNamespace ++ Permissions.permissionToJs(permissionName);

  let writePermission = (name, permission) => {
    let json =
      permission
      ->Belt.Option.getWithDefault(Permissions.Status.{state: `Prompt})
      ->Permissions.Status.encode
      ->Js.Json.stringify;
    let _ = name->getKey->LocalStorage.setItem(json);
    ();
  };

  let readPermission = name =>
    name
    ->getKey
    ->LocalStorage.getItem
    ->Belt.Option.flatMap(p =>
        try(p->Js.Json.parseExn->Js.Option.some) {
        | _ => None
        }
      )
    ->Belt.Option.flatMap(json =>
        switch (json->Permissions.Status.decode) {
        | Belt.Result.Ok(s) => s->Js.Option.some
        | Belt.Result.Error(_) => None
        }
      );
};

/**
 * A wrapper that emulates window.navigator.permissions in environments in which
 * it does not exist by persisting permission state to localstorage.
 */
module Make =
       (
         Conf: {
           type data;
           let name: Permissions.permission;
           let onPrompt:
             unit =>
             Js.Promise.t((option(Permissions.Status.t), option(data)));
         },
       ) => {
  let status = ref(Unknown);
  let handleStatusChange = newStatus => {
    status := newStatus;

    let _ =
      Webapi.Dom.(
        window
        |> Window.dispatchEvent(Event.make(permissionStatusChangeEvent))
      );
    ();
  };

  let prompt = () => {
    let newStatus =
      Conf.onPrompt()
      |> Js.Promise.then_(((perm, data)) => {
           let nextStatus =
             switch (perm->Belt.Option.map(Permissions.Status.state)) {
             | Some(`Granted) => PermissionGranted(data)
             | Some(`Denied) => PermissionRejected
             | _ => Unprompted
             };
           let _ = handleStatusChange(nextStatus);
           let _ = Storage.writePermission(Conf.name, perm);
           Js.Promise.resolve(nextStatus);
         })
      |> transitionInProgress;
    let _ = handleStatusChange(newStatus);
    newStatus;
  };

  let initialize = () => {
    let statusFromState = s =>
      switch (s->Permissions.Status.state) {
      | `Prompt => Unprompted
      | `Granted => prompt()
      | `Denied => PermissionRejected
      };
    let initialStatus =
      switch (Permissions.(query(Descriptor.make(~name=Conf.name)))) {
      | Some(p) =>
        p |> Js.Promise.then_(s => {s->statusFromState->Js.Promise.resolve})
      | None =>
        Storage.readPermission(Conf.name)
        ->Belt.Option.map(statusFromState)
        ->Belt.Option.getWithDefault(Unprompted)
        ->Js.Promise.resolve
      };

    initialStatus
    |> Js.Promise.then_(initialStatus => {
         handleStatusChange(initialStatus);
         Js.Promise.resolve(initialStatus);
       })
    |> transitionInProgress
    |> handleStatusChange;
  };

  let use = () => {
    let (internalStatus, setStatus) = React.useState(() => status^);
    let _ =
      React.useEffect0(() => {
        let handlePermissionStatusChange = _ev => {
          setStatus(_ => status^);
        };
        Webapi.Dom.(
          window
          |> Window.addEventListener(
               permissionStatusChangeEvent,
               handlePermissionStatusChange,
             )
        );
        let _ = setStatus(_ => status^);
        Some(
          () =>
            Webapi.Dom.(
              window
              |> Window.removeEventListener(
                   permissionStatusChangeEvent,
                   handlePermissionStatusChange,
                 )
            ),
        );
      });
    let onSetStatus = newStatus => {
      handleStatusChange(newStatus);
    };

    (internalStatus, prompt, onSetStatus);
  };
};

module Camera =
  Make({
    let name = `Camera;
    type data = MediaStream.t;

    let onPrompt = () =>
      MediaDevices.MediaConstraints.(
        make(~audio=true, ~video=makeVideo(~facingMode="environment"), ())
      )
      |> MediaDevices.getUserMedia
      |> Js.Promise.then_(stream => {
           let perm = Some(Permissions.Status.{state: `Granted});
           Js.Promise.resolve((perm, Some(stream)));
         })
      |> Js.Promise.catch(_ex => {
           let perm = Some(Permissions.Status.{state: `Denied});
           Js.Promise.resolve((perm, None));
         });
  });

module Geolocation =
  Make({
    let name = `Geolocation;
    type data = Externals_Geolocation.position;

    let onPrompt = () =>
      Js.Promise.make((~resolve, ~reject as _reject) =>
        Externals_Geolocation.getCurrentPosition(
          position => {
            let perm = Some(Permissions.Status.{state: `Granted});
            let _ = resolve(. (perm, Some(position)));
            ();
          },
          _positionError => {
            let perm = Some(Permissions.Status.{state: `Denied});
            let _ = resolve(. (perm, None));
            ();
          },
          Externals_Geolocation.currentPositionOptions(
            ~enableHighAccuracy=true,
            (),
          ),
        )
      );
  });
