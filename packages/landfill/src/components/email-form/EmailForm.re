open Styles;

Utils.require("./email-form.module.css");

let component = ReasonReact.statelessComponent("EmailForm");

let make = _children => {
  let handleSubmit = ev => {
    let _ = ReactEvent.Form.preventDefault(ev);
    Js.log2("handleSubmit", ev);
  };
  {
    ...component,
    render: _self =>
      <form
        className={cn(["flex", "flex-row", "relative"])}
        onSubmit=handleSubmit>
        <div
          style={make(~width=px(48), ())}
          className={cn([
            "absolute",
            "top-0",
            "bottom-0",
            "left-0",
            "flex",
            "justify-center",
            "items-center",
            "sd",
          ])}>
          <Icon.Phone style={make(~width=px(24), ~height=px(24), ())} />
        </div>
        <input
          type_="tel"
          placeholder="Phone Number"
          className={cn([
            "br1",
            "ba",
            "b--black",
            "pv_75",
            "pr_75",
            "pl3",
            "bg-transparent",
            "mr2",
            "flex-auto",
            "ff-poppins",
            "f13",
          ])}
        />
        <Button type_="submit" label={ReasonReact.string("Submit")} />
      </form>,
  };
};
