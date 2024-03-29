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

On Sourcehut, every mailing list uses `lists.sr.ht` as its email domain, and
every tracker uses `todo.sr.ht`. For example, a project made by `~john` could
have a mailing list at `~john/foobar@lists.sr.ht` and a tracker at
`~john/foobar@todo.sr.ht>`.

You can find the various mailing lists and trackers for Libvmm below:

* Patches & Discussion: [Mailing Lists](https://sr.ht/~satchmo/libvmm/lists)
* Bugs & Features:      [Trackers](https://sr.ht/~satchmo/libvmm/trackers)

## Patches

The general process for patch submission is as follows:

1. Clone the upstream repository.
2. Create a new branch.
3. Make your changes.
4. Commit your changes.
5. **Send out a patch to the appropriate mailing list.**

Step 5 is the only part that may be unfamiliar to users coming from other
workflows, so we'll focus on that. In short, `git-send-email` is your friend.
The command takes care of creating and sending out patches; all you have to do
is tell it where to send a patch and what commits should be included:

```sh
git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" <patch>
```

Here, `<patch>` is typically a commit ID or [revision
list](https://git-scm.com/book/en/v2/Git-Tools-Revision-Selection). If
`<patch>` is a commit ID, you can optionally use the flag `-<N>`, where `<N>`
is a number, to specify that you want to include the last `<N>` commits
starting at `<patch>`. If `-<N>` is specified but `<patch>` is not provided,
`HEAD` is used as the starting point.

After the patch is sent, a maintainer will review it and push it upstream if
everything checks out. If there's a problem with your patch, simply fix any
issues pointed out and send another version of the patch like so:

```sh
git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" -v2 <patch>
```

#### Comments

Often times, you'll want to include comments in a patch but not the final git
log. For instance, you may want to note that a particular patch fixes an issue
raised by a previous version of the patch. To do this, use the `--annotate`
option to open the patch in an editor so that you may modify it:

```sh
git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" --annotate -v3 <patch>
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

For complex changes, multiple patches may be necessary. In such cases, it's
common to have a cover letter to introduce or provide additional context for
the patches. To do so, use the `--cover-letter` option to create a separate
email that will be sent ahead of your patches:

```sh
git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" --cover-letter --annotate <patchset>
```

Note that we must also use the `--annotate` option, as we need to edit the
cover letter's "Subject" header appropriately.

## Resources

* For a tutorial on sending patches: [git-send-email](https://git-send-email.io/)
* For general guidelines on email etiquette: [man.sr.ht](https://man.sr.ht/lists.sr.ht/etiquette.md)
* For more information on Git revision lists: [git-scm](https://git-scm.com/book/en/v2/Git-Tools-Revision-Selection)
