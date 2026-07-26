<!-- ai-project-init:begin managed -->
# Linear Instructions

Preferred team: Not configured
Preferred project: Not configured
Last Linear access test: 2026-07-25T20:49:10.377298+00:00
Linear access result: ok
Linear access detail: Linear API authenticated as Dave.

## Creating Linear Issues

- Write issue titles and descriptions so an outsider with no knowledge of prior chat context can understand the task and execute it.
- Include enough background, scope, constraints, expected behavior, and acceptance criteria for the issue to stand on its own.
- Prefer concrete implementation-oriented descriptions over brief reminders or shorthand.
- If relevant, include current behavior, desired behavior, why the change is needed, important files/systems/workflows, and validation or acceptance criteria.

## Issue Sizing And Relationships

- Prefer smaller, well-scoped issues over large monolithic issues.
- If work naturally breaks into multiple meaningful steps, create separate issues and connect them logically.
- Use parent/child relationships when a larger body of work should be organized into smaller deliverables.
- Use blocking relationships when one issue materially prevents progress on another.
- Add relationships between related issues when they help planning or execution.
- Do not force relationships when there is no real dependency or coordination value.

## Reading And Working Linear Issues

When addressing a Linear issue, review all relevant issue context before implementation:

- Read the full issue description.
- Read all comments.
- Review related issues that may affect the work.
- Review attached images in the issue description or comments.
- Prefer plans that address a parent issue and its child issues together instead of planning each child separately.

Assume important instructions may appear in comments, related issues, or image attachments, not only in the main issue body.

## Linear Status Rules

- New issues: `TODO`
- Work started: `In Progress`
- Work addressed and ready for human review: `In Review`

The human user is responsible for moving issues to a done/completed state after reviewing the work.
The agent is responsible for updating issue statuses as it progresses through development and for adding progress comments.
If all child issues are set to the same status, set the parent issue to the same status.

## Practical Expectations

- Do not mark an issue as done/completed.
- Before implementation, confirm that the issue scope still matches its latest description, comments, attachments, and linked context.
- When creating new issues, organize them so they form a coherent plan instead of an unstructured backlog.
<!-- ai-project-init:end managed -->
