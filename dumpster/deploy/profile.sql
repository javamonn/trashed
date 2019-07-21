-- Deploy dumpster:profile to pg

BEGIN;

  create table dumpster.profile (
    id uuid not null default uuid_generate_vm1c() primary key,
    phone_number text not null unique,
    created_at timestamptz not null default now()
  );

COMMIT;
