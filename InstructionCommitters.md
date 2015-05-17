

# Introduction #

The main advantage of the open-source software is that you may not only use it, but also modify it. We provide detailed instructions for modification of some parts of `ADDA` to add additional functionality on separate pages (see the section "Adding new code" of the left-side tab), including, for example, adding [new shapes](AddingShape.md) and [incident beams](AddingBeam.md). However, you should not limit your efforts to these parts, if you are not afraid of somewhat larger complexity. You may also want to help the developers and fix some nasty bug in `ADDA`.

In any case, if you add code or change something in `ADDA`, please consider contributing your code to be incorporated in future releases. The rest of this page provides instructions for such contribution. Moreover, it is best to plan your contribution _before_ the coding. In particular, please first search the [issue tracker](http://code.google.com/p/a-dda/issues/list) for the corresponding issue. If no suitable issue is found, submit a new one. A careful description of an issue with possible additions or changes from other developers is a key to easy incorporation of the new features into the main `ADDA` code. It is also recommended to study the [structure of the existent code](CodeDesign.md) to better place your efforts.

Before you contribute your code, please try to make it readable and include enough comments, so the others may understand and modify it. There are many guides on good coding practices (see, e.g., [1](http://www.teamten.com/lawrence/style/notes.html) and [2](http://www.ibm.com/developerworks/aix/library/au-hook_duttaC.html)), which to some extent contradict each other. A good idea is to adhere to the [code style](CodeStyleGuide.md), which is already used throughout the code.

By contributing the code, you naturally accept that it will be distributed under [GPL 3 license](http://www.gnu.org/licenses/gpl.html). Additionally, please put a copyright notice in header of each new source file (which you created yourself). You can find example of such notice in any existing source file. You will need a [Subversion client](http://subversion.apache.org/packages.html) to work with the code repository. The instructions below are given for a command-line client, but using GUI client is also possible. See also [Subversion FAQ for Google Code](http://code.google.com/p/support/wiki/SubversionFAQ) and [Subversion book](http://svnbook.red-bean.com/index.en.html).

Particular actions required to contribute your code depend on the size and nature of the contribution and are described below.

# Submitting a patch #

This way is recommended if your piece of code is relatively simple, addresses a particular issue, and should not interfere with other parts of `ADDA` code. The action plan is the following:
  * Get the current source
```
svn checkout http://a-dda.googlecode.com/svn/trunk/ adda
```
  * Implement changes in the code.
  * Test that the code compiles and its behavior has been changed as expected.
  * Create a patch file by positioning in `adda/` and typing
```
svn diff > patchfile
```
  * Submit `patchfile` as a comment (attachment) to the issue, which it addresses. If you have added new files to the source, include these files as well.

Developers will evaluate the patch and, if considered suitable, apply it. Your contribution will be acknowledged by comments in the code and at [Acknowledgements](Acknowledgements.md).

# Becoming an official contributor #

This is recommended if you want to submit several patches or implement a complex new feature. To become `ADDA` contributor you need a Google account (you may use Gmail, or tie it to any other e-mail address) and to contact [Maxim Yurkin](mailto:yurkin@gmail.com) describing your plans for new code. Please note that your account name may be visible on some `ADDA` web pages (also you can control it to some extent at [Google Code settings](https://code.google.com/hosting/settings)).

Next, your account name will be added to the project, and will be included in the [people list](http://code.google.com/p/a-dda/people/list). Please also add a short description of yourself to the right column of this list. You will also be subscribed to the [developers mailing list](http://groups.google.com/group/adda-develop) to be automatically notified of changes to the source code. Being a contributor adds some responsibility since you have access to the source code, issue tracker, and wiki pages. But do not worry - the main idea of the version control system is that anything can be undone. So you can not possibly break anything.

There are no strict rules on how to commit a new code, for instance, whether a large feature should be committed at once or it can be separated into several commits. A reasonable behavior is to commit as often as possible, but so that each commit is operational. In other words, please try to ensure that after each new commit the code at least compiles and its basic functions (unaffected by the new features) work as expected. In the following two possible examples of workflow are described.

## Committing minor changes ##

The procedure is similar to submitting a patch (see above), but you commit the changes directly to the repository.
  * Checkout the current source
```
svn checkout https://a-dda.googlecode.com/svn/trunk/ adda --username <your username>
```
> When prompted, enter your [generated Google Code password](http://code.google.com/hosting/settings). In the following it is assumed that Subversion client have saved your credentials and you do not need to enter them again.
  * Implement changes in the code.
  * Test that the code compiles and its behavior has been changed as expected.
  * If you have added new files to the source, explicitly add them to version control
```
svn add <filename1> <filename2> ...
```
> If you add text source files (typical), also set svn properties. The default values, used throughout the `ADDA` source, can be applied as follows
```
svn propset svn:eol-style native <filename1> <filename2> ...
svn propset svn:keywords "Author Date Id Revision" <filename1> <filename2> ...
```
  * Submit the modifications back into the repository by positioning in `adda/` and typing
```
svn commit -m "<commit log message>"
```
> The larger the commit log message - the better. You may look at [existing logs](http://code.google.com/p/a-dda/source/list) for examples. If the commit fixes a particular issue you may include something like "`Fixes issue <#>.`" in the message. Google Code will then automatically update the issue description ([details](http://code.google.com/p/support/wiki/IssueTracker#Integration_with_version_control)).
  * Update affected documentation (wiki pages) and issues.

You may discover a minor fault (typo) in your code after the commit. Then do not hesitate to correct it immediately and commit again. It is better to have too many (minor) commits than to wait with correction till the next major commit, which makes it possible to forget about the correction at all. It is the final version that matters.

If you want to perform another code change, repeat the above steps but instead of `'svn checkout'` use
```
svn update
```
while positioned in `adda/`. It will check the repository for any changes introduced by other developers and update your working copy if needed.

## Implementing larger changes ##

If you need to implement a large feature, which will modify different parts of the code and probably affect even more parts, a good idea is to separate it into several commits. This will facilitate code review and testing by other developers, providing the new code in manageable chunks. However, the code in between these commits may be hardly operational. To address this problem, it is recommended to make large changes on a separate branch (in contrast to committing changes directly to so-called `trunk` as discussed above). The overall procedure is the following.
  * Create (fork) a new branch from the current source
```
svn copy -m "Created new branch <branch_name> for <branch description>" \
    https://a-dda.googlecode.com/svn/trunk https://a-dda.googlecode.com/svn/branches/<branch_name>
```
  * Checkout the branch to a new location, not to mix it up with the working copy of the trunk
```
svn checkout https://a-dda.googlecode.com/svn/branches/<branch_name> adda_branch
```
  * Modify the code inside this branch and commit it as often as needed, as described [above](InstructionCommitters#Committing_minor_changes.md).
  * If during your work the trunk is changed by other developers, you may want to merge their changes into your branch to minimize future conflicts. The best time to do it is after the next commit on the branch, when there are no local changes in your working copy. Positioning in `adda_branch/` type
```
svn merge https://a-dda.googlecode.com/svn/trunk
```
> and examine the merged changes for possible conflicts by both examining the source code directly (`'svn diff'` may help) and testing the compiled program. After taking care of conflicts, if needed, submit the changes on your branch to the repository
```
svn commit -m "Merged latest trunk changes to branch <branch_name>."
```
  * After you made all changes to the code on the branch perform all tests, you can think of, and wait for other developers to review the new code.
  * Make final merge of possible changes in `trunk` to your branch (see two steps above).
  * Finally, merge your branch back into the trunk, as following. Go back to the working copy of the trunk (or checkout one), e.g. `adda/` update it, and merge changes
```
svn update
svn merge --reintegrate https://a-dda.googlecode.com/svn/branches/<branch_name>
```
> Search for all kind of conflicts that can appear and resolve them; compile and test the program. Next, commit the changes back into trunk
```
svn commit -m "Merge branch <branch_name> back into trunk."
```
  * Delete the branch. Note that the history of your branch and corresponding changes will not be lost.
```
svn delete https://a-dda.googlecode.com/svn/branches/<branch_name> \
    -m "Remove branch <branch_name>, reintegrated with trunk in r<###>."
```

See [the chapter of Subversion book](http://svnbook.red-bean.com/en/1.6/svn.branchmerge.html) for more details on branching and merging.

# Contributing additional tools, scripts, etc. #

If you intensively use `ADDA` in your research, you probably have devised some tricks to make this process faster, more convenient, and less error-prone. Or you may have been so desperate to wait for implementation of a particular new `ADDA` feature that you devised a dirty but effective workaround. Either way, if you have something that anyhow enhances `ADDA` functionality, please consider sharing it with other users.

There is a special place in [ADDA package](PackageDescription.md) for this purpose - [misc/](http://code.google.com/p/a-dda/source/browse/#svn%2Ftrunk%2Fmisc) directory. It is distributed with `ADDA` but is not tightly integrated with the main code. The latter implies two advantages:
  * Almost anything can be contributed: bash or python scripts, Windows `*.bat` files, source codes in C, Fortran, Matlab, Mathematica, etc., even binary executables.
  * You do not need to worry about the future `ADDA` development and backward compatibility.

Of course, the amount of potential users of your tool depends on its portability (supported operating systems and whether it requires proprietary software to run) and amount of time you invested to polish the tool and its documentation. But even if you contribute the version, which you use yourself, it may benefit other people. The tool does not need to be perfect from the beginning. Moreover, as soon as you put your tool in the `ADDA` repository other people may also improve it.

The easiest way to contribute your tool is to give it as-is through the [discussion group](http://groups.google.com/group/adda-discuss), [issue tracker](http://code.google.com/p/a-dda/issues/list), or contacting [developers](http://code.google.com/p/a-dda/people/list) directly. The developers will place your tool in an appropriate place inside `misc/`. We only ask that you include a short description of the tool, e.g. in a text file. Please also specify examples of usage and a particular version of `ADDA`, with which the tool was used (tested). If you have a relevant publication, in which your tool is described or used, include it in the description and ask others to cite your paper when they use your tool.

However, if you can invest a little bit more time, please consider [becoming the official contributor](InstructionCommitters#Becoming_an_official_contributor.md). After that you can commit your tool directly and make any changes in the future. The additional effort required from you is quite small since you do not need to worry about interaction with other developers (branches, etc.). In other words, you directory inside `misc/` is like a branch itself. The procedure is similar to the one described [above](InstructionCommitters#Committing_minor_changes.md), the only difference is that all changes are localized inside `misc/` directory.
  * Checkout the current `misc/` directory
```
svn checkout https://a-dda.googlecode.com/svn/trunk/misc/ --username <your username>
```
  * Add a new directory for your tool
```
cd misc
svn mkdir <tool_name>
```
  * Copy all the files relevant for your tool into `misc/<tool_name>/`
  * Schedule new files for addition and commit everything (including directory `<tool_name>` itself)
```
svn add <filename1> <filename2> ...
cd ..
svn commit -m "Contributed <tool_description> to misc/<tool_name>"
```
  * Update affected documentation (wiki pages) and issues, especially if your tool provides some kind of workaround.

If you want to make any changes afterwards, just follow the [instructions for minor changes](InstructionCommitters#Committing_minor_changes.md) while positioned in `misc/<tool_name>/`. Use of `svn update` may still be required before committing a new set of changes, even when changes of other developers (between your commits) does not affect `misc/<tool_name>/` at all.