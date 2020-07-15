# **Contributing**

## Workflow

Contributing to Libvmm is done primarily through email, which means that if you
have an email address and know how to send emails, you're already familiar with
90% of Libvmm's development workflow! The other 10% can be broken down into two
things: knowing where to send emails and knowing how to work with patches.

#### Mailing Lists and Trackers

In Sourcehut, emails go to one of two places: mailing lists and trackers.
Generally, mailing lists are used for patches and discussions while trackers
are used for issues or feature requests.

There may be multiple mailing lists or trackers for a single project, but
they'll each share a common email domain: mailing lists use `lists.sr.ht` and
trackers use `todo.sr.ht`. For example, patches for Libvmm should be sent to
<~satchmo/libvmm-devel@lists.sr.ht>, and bug reports to
<~satchmo/libvmm@todo.sr.ht>.

You can find the various mailing lists and trackers for Libvmm below:

* Patches & Questions: [Mailing Lists](https://sr.ht/~satchmo/libvmm/lists)
* Bugs & Features:     [Issue Trackers](https://sr.ht/~satchmo/libvmm/trackers)
<!--* Documentation:       [libvmm-docs](https://man.sr.ht/~satchmo/libvmm-docs)-->

<!--If you still don't want to use email for your contributions, no worries.-->
<!--There's also a web interface for each mailing list and tracker. You can find-->
<!--these using the links above.-->

#### Patches

Now let's talk about patches. The general process for patch submission is as
follows: clone the upstream repository, make your changes, commit them, and
send out a patch to the appropriate mailing list. The first three steps should
be familiar, so we'll focus on the last one.

To send out a patch, `git send-email` can be used. This command creates a patch
and sends it out in a single step; all you have to do is tell it where to send
the patch and which commits should be included:

```sh
$ git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" <patch>
```

Note that a "patch" can actually refer to a number of things. But more often
than not, a revision list is what you'll be using. See the [Resources](#resources)
section for more information on revision lists.

##### Version Numbers

After review, you may be asked to fix up or add things to your patch (perhaps
there's conflicts). If you find yourself in this situation, simply fix whatever
issues there are and send out another version of the patch using the `-v<N>`
option, where `<N>` is the version number of the patch:

```sh
$ git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" -v2 <patch>
```

##### Comments

Often times, you'll also want to include comments in a patch but not the final
git log. For instance, you may note that a particular patch fixes an issue
raised by a previous version of the patch. To do this, use the `--annotate`
option to open the patch in an editor so that you may modify it:

```sh
$ git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" --annotate -v3 <patch>
```

Once in the editor, add your comments after the "---" mark:

```console
Subject: [PATCH v3] Fix typo in README

---
This fixes issues raised in the second patch.

README.md | 1 +
...
```

##### Cover Letters

It's common to have a cover letter for more complex patches in order to provide
an introduction or some context for the patch. To do this, use the
`--cover-letter` option to create a separate email that will be sent ahead of
your actual patch:

```sh
$ git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" --cover-letter --annotate <patch>
```

Note that we must also use the `--annotate` option, as we need to edit the
cover letter's "Subject" header appropriately:

#### Resources

* For a tutorial on sending patches: [git-send-email](https://git-send-email.io/)
* For general guidelines on email etiquette: [man.sr.ht](https://man.sr.ht/lists.sr.ht/etiquette.md)
* For more information on Git revision lists: [git-scm](https://git-scm.com/book/en/v2/Git-Tools-Revision-Selection)
