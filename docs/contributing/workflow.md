# Workflow

Contributing to Libvmm is done primarily through email, which means that if you
have an email address and know how to send emails, you're already familiar with
90% of Libvmm's development workflow! The other 10% can be broken down into two
things:

* Knowing where to send emails.
* Knowing how to work with patches.

## Mailing Lists and Trackers

In Sourcehut, emails go to one of two places: mailing lists or trackers.
Generally, mailing lists are used for patches and discussions while trackers
are used for issues and feature requests.

There may be multiple lists or trackers for a single project, but each
destination type has its own email domain that is used across all Sourcehut
projects: mailing lists use `lists.sr.ht` and trackers use `todo.sr.ht`. As an
example, a project of user `~user` could have a mailing list at
`~user/foobar@lists.sr.ht` and a tracker at `~user/foobar@todo.sr.ht>`.

You can find the various mailing
lists and trackers for Libvmm below:

* Patches & Discussion: [Mailing Lists](https://sr.ht/~satchmo/libvmm/lists)
* Bugs & Features:      [Trackers](https://sr.ht/~satchmo/libvmm/trackers)

## Patches

The general process for patch submission is as follows:

1. Clone the upstream repository.
2. Create a new branch.
3. Make your changes.
4. Commit your changes.
5. **Send out a patch to the appropriate mailing list.**
6. Delete the branch after patch is accepted.

Step 5 is the only part that may be unfamiliar to users coming from other
workflows, so we'll focus on that. To send a patch, `git-send-email` is often
used. The command takes care of creating and sending out patches; all you have
to do is tell it where to send the patch and what commits should be included:

```sh
git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" <patch>
```

Here, `<patch>` is typically a commit ID or [revision
list](https://git-scm.com/book/en/v2/Git-Tools-Revision-Selection). You can
also omit `<patch>` and use the flag `-<N>`, where `<N>` specifies the last `N`
commits in the current branch.

After the patch is sent, it is eventually reviewed and pushed upstream by a
maintainer if everything checks out. If there's a problem, simply fix any
issues pointed out and send out another version of the patch like so:

```sh
git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" -v2 PATCH
```

#### Comments

Often times, you'll want to include comments in a patch but not the final git
log. For example, you may want to note that a particular patch fixes an issue
raised by a previous version of the patch. To do this, use the `--annotate`
option to open the patch in an editor so that you may modify it:

```sh
git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" --annotate -v3 PATCH
```

Once in the editor, add your comments after the `---` mark:

```console
Subject: [PATCH v3] Fix typo in README

---
This fixes issues raised in the second patch.

README.md | 1 +
...
```

#### Cover Letters

It's common to have a cover letter for patchsets (a collection of patches) to
introduce or provide some context for the various patches. If you want to
include a cover letter, use the `--cover-letter` option to create a separate
email which will be sent ahead of your actual patch:

```sh
git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" --cover-letter --annotate <patchset>
```

Note that we must also use the `--annotate` option, as we need to edit the
cover letter's "Subject" header appropriately.

## Resources

* For a tutorial on sending patches: [git-send-email](https://git-send-email.io/)
* For general guidelines on email etiquette: [man.sr.ht](https://man.sr.ht/lists.sr.ht/etiquette.md)
* For more information on Git revision lists: [git-scm](https://git-scm.com/book/en/v2/Git-Tools-Revision-Selection)
