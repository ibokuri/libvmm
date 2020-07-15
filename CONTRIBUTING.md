# **Contributing**

## **Workflow**

Contributing to Libvmm is done largely through email, which means if you have
an email address and know how to send emails, congratulations! You already know
90% of Libvmm's development workflow! The other 10% can be broken down into
knowing where to send emails and how to work with patches.

### Mailing Lists and Trackers

In Sourcehut, emails go to one of two places: mailing lists or trackers.
Generally, mailing lists are used for patches and discussion while trackers are
used for issues or feature requests.

There may be multiple mailing lists and trackers for a single project, but
they'll each share a common email domain: mailing lists use `lists.sr.ht` and
trackers use `todo.sr.ht`. As an example, patches for Libvmm should be sent to
<~satchmo/libvmm-devel@lists.sr.ht> and bug reports to
<~satchmo/libvmm@todo.sr.ht>.

You can find the various mailing lists and trackers for Libvmm below:

* Patches & Questions: [Mailing Lists](https://sr.ht/~satchmo/libvmm/lists)
* Bugs & Features:     [Issue Trackers](https://sr.ht/~satchmo/libvmm/trackers)
<!--* Documentation:       [libvmm-docs](https://man.sr.ht/~satchmo/libvmm-docs)-->

If you still don't want to use email for your contributions, no worries.
There's also a web interface for each mailing list and tracker. You can find
these using the links above.

### Patches

Now let's talk about patches. The general process for patch submission is as
follows: clone the upstream repository, make your changes, commit them, and
send out a patch to the appropriate mailing list.

The last part can be easily done with `git send-email`, which creates a patch
for you and sends it off in a single step. All you have to do is tell it where
to send the patch and which commits should be included:

```sh
$ git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" <git revision>
```

If, after review, your patch needs some work (perhaps there's conflicts),
simply fix whatever issues there are and send out another version of the patch:

```sh
$ git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" -v2 <git revision>
```

Often times, you'll want to add some comments to a patch, but don't necessarily
want such information in the final git log. In these cases, use the
`--annotate` flag, which will open up the email in an editor so that you may
modify it. Add your comments below the "---" symbol.

```sh
$ git send-email --to="~satchmo/libvmm-devel@lists.sr.ht" --annotate -v3 <git revision>
```

```console
Subject: [PATCH v3] Fix typo in README

---
This fixes issues raised in the second patch.

README.md | 1 +
...
```

### Resources

* For a tutorial on sending patches: [git-send-email](https://git-send-email.io/)
* For general guidelines on email etiquette: [man.sr.ht](https://man.sr.ht/lists.sr.ht/etiquette.md)
* For more information on Git revisions: [git-scm](https://git-scm.com/book/en/v2/Git-Tools-Revision-Selection)
