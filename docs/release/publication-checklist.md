# Public Publication Checklist

Before pushing this repository publicly:

- Do not commit signing certificates, private keys, provisioning profiles,
  notary credentials, `.env` files, or local publish configs.
- Do not commit the local Adobe SDK payloads unless you have redistribution
  rights from Adobe.
- Confirm `git status --ignored` shows SDK payloads and generated outputs as
  ignored, not staged.
- Build packages locally, then sign and notarize the generated `.pkg` outside
  the source repository.
- Publish release packages separately from source if needed.

Useful checks:

```sh
find . -type f \( -iname '*.p12' -o -iname '*.p8' -o -iname '*.pem' -o -iname '*.key' -o -iname '*.cer' -o -iname '*.mobileprovision' \) -print

rg -n --hidden --glob '!.git/**' 'Developer ID|notarytool|keychain-profile|BEGIN .*PRIVATE KEY|password|api[-_ ]?key|issuer'
```
