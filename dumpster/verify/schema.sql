-- Verify db:schema on pg

BEGIN;

  SELECT pg_catalog.has_schema_privilege('dumpster', 'usage');
  SELECT pg_catalog.has_schema_privilege('dumpster_private', 'usage');

ROLLBACK;
