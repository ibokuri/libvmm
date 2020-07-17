# **Contributing**

## Workflow

Contributing to Libvmm is done primarily through email, which means that if you
have an email address and know how to send emails, you're already familiar with
90% of Libvmm's development workflow! The other 10% can be broken down into two
things: knowing where to send emails and knowing how to work with patches.

#### Mailing Lists and Trackers

In Sourcehut, emails go to one of two places: mailing lists or trackers. Each
destination type has its own email domain, which is used across all Sourcehut
projects: mailing lists use `lists.sr.ht` and trackers use `todo.sr.ht`. For
example, patches for Libvmm should be sent to
<~satchmo/libvmm-devel@lists.sr.ht>, and bug reports to
<~satchmo/libvmm@todo.sr.ht>.

Generally, mailing lists are used for patches and discussions while trackers
are used for issues or feature requests. You can find the various mailing
lists and trackers for Libvmm below:

* Patches & Questions: [Mailing Lists](https://sr.ht/~satchmo/libvmm/lists)
* Bugs & Features:     [Issue Trackers](https://sr.ht/~satchmo/libvmm/trackers)

#### Patches

The general process for patch submission is as follows:

1. Clone the upstream repository.
2. Make your changes.
3. Commit your changes.
4. Send out a patch to the appropriate mailing list.

The first three steps are easy enough, so we'll focus on the last one. To send
a patch, `git send-email` is your friend. It creates and sends out a patch; all
you have to do is tell it where to send the patch and what should be included:

```sh
git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" PATCH
```

where `PATCH` is typically a commit id, revision list, or the flag `-<N>`
(where `<N>` is the last `N` commits in the current branch). For more
information, see the [Resources](#resources) section.

Once the patch hits the mailing list, a maintainer will review it and push it
upstream if everything looks okay. If there's a problem with the patch, simply
fix whatever issues are pointed out and send out another version of the patch
using the `-v<N>` option, where `<N>` is the version number of the patch:

```sh
git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" -v2 PATCH
```

And now you know how to send patches! To end this section, I'll leave you with
a few common patch-related operations:

* **Comments**: Often times, you'll also want to include comments in a patch but
  not the final git log. For instance, you may note that a particular patch
  fixes an issue raised by a previous version of the patch. To do this, use the
  `--annotate` option to open the patch in an editor so that you may modify it:

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

* **Cover letters**: It's common to have a cover letter for patchsets (a
  collection of patches) in order to provide an introduction to or some context
  for the various patches. If you want to include a cover letter, use the
  `--cover-letter` option to create a separate email which will be sent right
  before your actual patch:

  ```sh
  git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" --cover-letter --annotate <patchset>
  ```

  Note that we must also use the `--annotate` option, as we need to edit the
  cover letter's "Subject" header appropriately.

#### Resources

* For a tutorial on sending patches: [git-send-email](https://git-send-email.io/)
* For general guidelines on email etiquette: [man.sr.ht](https://man.sr.ht/lists.sr.ht/etiquette.md)
* For more information on Git revision lists: [git-scm](https://git-scm.com/book/en/v2/Git-Tools-Revision-Selection)
