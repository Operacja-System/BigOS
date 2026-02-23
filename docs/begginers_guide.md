# Begginer's guide

This `.md` file is a starting point for anyone's journey with the BigOS project. Containing advice from hints for basic procedures *(e.g. What should be in a PR?)* to links and FAQ.

## Repository structure

In the main directory **95%** of Your work will concern the [src](../src/) and [include](../include/) directories.

The [CMake](../CMake/) directory contains CMake config files and toolchain for the [RISC-V](https://en.wikipedia.org/wiki/RISC-V) architecture.

The [external](../external/) directory contains third-party dependencies, we try to use as little external code as possible, so if external code is required, its inclusion must be clearly justified.

The [scripts](../scripts/) directory contains useful scripts to check code cleanliness and maintain style consistency.

The [docs](.) directory *(You're here!)* contains the project documentation, rules and additional information to help You navigate and collaborate effectively.

Lastly, the [src](../src/) and [include](../include/) directories – where  **95%** of Your work will take place.

## Pull request workflow

A pull request *should* concern a single, specific functionality, in specific cases a bundle of functionalities in a single PR may be requested but again – it needs to be justified! Commit names and descriptions should be substantive *(no "Minor changes")*. This also concerns the pull request Assignees, labels, etc.

### Opening a draft PR

Unless your implementation is ready, consider opening the pull request as a draft first. This signals to the team that:

1. Someone is working on the topic
2. It's **not** finished yet
3. With clear documentation of progress and plans, it allows for others e.g. with dependencies on your work to predict when it will be finished

### Opening a pull request

After finishing the implementation ensure the following:

- Verify that the implementation works, try to test it as best you can

- The documentation covers everything that was changed
  
- All GitHub actions pass

- Clean up commits if necessary

If everything is completed You're free to request a review.

### Reviews and Revisions

According to the [contributing rules](./contributing_rules.md#git), at least one reviewer needs to approve a pull request before merging it to the main repository.

If a reviewer requests changes, address all comments by either explaining your implementation or making the necessary adjustments.

The comments may concern code comprehensibility or non-critical details, if something is unclear, try to provide an explanation to others.

Even if the pull request is accepted, make sure all conversations are resolved before merging.

### Merging

After the pull request has been approved and all conversations are resolved, You can merge the changes into the main branch. If merge conflicts occur, consult the team members responsible for the affected files.

If You're working with branches, delete the branch after merging, unless it is intended for ongoing work.

## Reviewing Pull Requests

Careful review of other's code is just as important as writing Your own. As a reviewer, follow these guidelines:

- Familiarize Yourself with the context first

- Ensure the implementation works logically

- Check for clarity, readability, and maintainability

- Look for potential bugs, edge cases, or missing error handling

- Provide constructive feedback

- Ask questions if something is unclear

- If changes are needed, clearly describe what should be improved

- Respond to the author’s updates and verify fixes

- Approve the PR if it meets quality standards

## FAQ

***Feel free to ask questions :) 🧃***

## Discussions

Don't be afraid to discuss more freely on the Discord server if you have access. Many useful resources were already found and plans have been made. Until they get added to the docs, that's where You'll find them.
