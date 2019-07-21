-- Deploy db:schema to pg

BEGIN;

  create schema dumpster;
  create schema dumpster_private;

COMMIT;
