<!-- ai-project-init:begin managed -->
# Secrets And Security

- Never print, commit, or summarize raw secrets, tokens, private keys, cookies, or authorization headers.
- Treat `.env`, key files, credentials, certificates, and secret-bearing logs as sensitive until proven otherwise.
- Prefer environment variables, local secret stores, or platform-managed secrets over checked-in credentials.
- When touching auth, authorization, data export, webhooks, or dependency boundaries, include a security review note.
- If a file appears to contain a secret, stop using the value and report the file path and risk without repeating the secret.
<!-- ai-project-init:end managed -->
