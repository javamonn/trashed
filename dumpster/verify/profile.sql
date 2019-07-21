-- Verify dumpster:user on pg

BEGIN;

  select * from bigspring.profile where false;

ROLLBACK;
