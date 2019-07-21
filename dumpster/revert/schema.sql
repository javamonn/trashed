-- Revert db:schema from pg

BEGIN;

  drop schema if exists dumpster;
  drop schema if exists dumpster_private;

COMMIT;
