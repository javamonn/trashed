-- Revert dumpster:user from pg

BEGIN;

  drop table if exists dumpster.profile;

COMMIT;
