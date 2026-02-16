# Security Policy

This file describes how security issues are reported and handled, and what the
expectations are for security issues reported to this project.


## Supported Versions

All production releases of this software are subject to this security policy.  A
production release is tagged and given a semantic version number of the form:

    MAJOR.MINOR.PATCH

where "MAJOR" is an integer starting at 1 and "MINOR" and "PATCH" are integers
starting at 0.  A feature release has a "PATCH" value of 0, for example:

    1.0.0
    1.1.0
    2.0.0

Beta releases and release candidates are *not* prodution releases and use
semantic version numbers of the form:

    MAJOR.MINORbNUMBER
    MAJOR.MINORrcNUMBER

where "MAJOR" and "MINOR" identify the new feature release version number and
"NUMBER" identifies a beta or release candidate number starting at 1, for
example:

    1.0b1
    1.0b2
    1.0rc1


## A Vulnerability vs A Bug

Based on the recent focus on security in general, we have to define criteria
for issues which will be handled as vulnerabilities and will go via vulnerability
process.


### Rule of Thumb

> Any issue which require CUPS admin action to trigger it is a bug in most cases.
> Groups which users are authorized to be CUPS admin are defined by the directive
> SystemGroup in cups-files.conf. Examples are below.

The following (but not restricted to) issues are highly probable to be handled as
vulnerability by OpenPrinting:

* issues with security related exploit provided by researcher - e.g.:
 * attacker remotely causes crash of daemon which runs in trusted network,
 * attacker exercises remote code execution in SW provided by OpenPrinting,
 * attacker exploits privilege escalation e.g.:
  * unauthenticated user has access to content/operations which require authentication,
  * user is able to execute operation they are not authorized for.\
* issues showing common weaknesses (buffer overflow, use-after-free, divide-by-zero etc.)
  without known exploit, but can be proven by e.g. address sanitizer - in general issues which
  pose security threat if exploited.

The following (but not restricted to) issues will be handled as bug:

* issues which require involment of CUPS admin - e.g.:
 * CUPS admin enters malformed input into configuration which is available for admin-only which
   triggers the bug
 * root misconfigures DNS entries which triggers issue in SW.
* memory leaks reported by address sanitizer for one-shot binaries (binaries which are meant to run
  once - then memory is collected by OS)

OpenPrinting group reserves the final decision how the report will be handled for its members.
If the issue is evaluated as bug, it will be either fixed and made public, or it will be reported
as an issue at relevant OpenPrinting Github project.


## Reporting a Vulnerability

Github supports private security advisories and OpenPrinting CUPS enabled
their usage, report all security issue via them. Reporters can file a security
advisory by clicking on `New issue` at tab `Issues` and choose
`Report a vulnerability`.  Provide details, impact, reproducer, affected
versions, workarounds and patch for the vulnerability if there are any and
estimate severity when creating the advisory.

Expect a response within 5 business days.


## How We Respond to Vulnerability Reports

First, we take every report seriously.  There are (conservatively) over a
billion systems using CUPS, so any security issue can affect a lot of people.

Members of the OpenPrinting security team will try to verify/reproduce the
reported issues in a timely fashion.  Please keep in mind that many members of
the security team are volunteers or are only employed part-time to maintain
CUPS, so your patience is appreciated!


### The vulnerability is in a different project

Sometimes a reported issue is actually in another project's code.  For these
issues we may ask you to re-submit your report to the correct project - an
enhancement request has been submitted to GitHub to correct this limitation for
projects hosted on GitHub.


### CVSS base metrics and CVE severity

Other times we may verify the issue exists but disagree on the severity or
scope of the issue.  We assess vulnerabilities based on our supported platforms
and common configurations because we need to be able to test and verify issues
and fixes on those supported platforms.

Github uses CVSS base metrics which require the default configuration of the affected
project.  This is the most important for attack vector metric in CVSS - because f.e.
CUPS listens on localhost and domain socket in default configuration, therefore
CVSS base metrics will reflect it.

The final CVSS score will later affect how the vulnerability is disclosed.


### Further steps

Similar issues (if multiple vulnerabilities are reported) will be combined if
they share a common root cause.  We don't mean any disrespect by doing this, we
just want to make sure your issues are truly and efficiently addressed in full.

Once we have verified things, we will work towards providing a fix as quickly
as possible.  Fixes are typically developed against the "master" branch, then
backported as needed to cover shipping CUPS releases on our supported platforms.

Once we have the fixes ready, we request a CVE, and based on CVSS score and exploit's
nature we handle the vulnerability:

1. Under embargo for vulnerabilities with CVSS score >= 7.0 and remotely exploitable vulnerabilities, or,
2. Push and announce the lower rated vulnerabilities (under CVSS < 7.0) without embargo process.

The lower rated vulnerability can be shipped under embargo if one of the key stakeholders
(Linux distributions, OS vendors) requests to do so.


## Responsible Disclosure

As mentioned above, a security issue can be handled under embargo or pushed into
project.  The issue and fix are shared amongst and reviewed by the key stakeholders
(Linux distributions, OS vendors, etc.) and the CERT/CC.


### Embargo process

If the issue goes via embargo, OpenPrinting group coordinates an embargo date, and announce it
on `distros@vs.openwall.org` mailing list.  The embargo period is typically 7-10 days long
but can be longer.

The embargo starts a flurry of activity - hundred of developers supporting every
Linux distribution, the various BSD flavors, macOS, and ChromeOS will queue up
the security updates for their respective OS releases on the public date.

On the public date, OpenPrinting group sends public announcement of the vulnerability
on `oss-security@lists.openwall.com` with the link to the fix, and release a new version
of the project which contains the security fix.

> Responsible disclosure applies only to production releases.  A security
> vulnerability that only affects unreleased code can be fixed immediately
> without coordination.  Vendors *should not* package and release unstable
> snapshots, beta releases, or release candidates of this software.


### Push And Announce of low-rated vulnerabilities

The issues with CVSS lower than 7.0 and without known exploit will go public
without embargo unless it is decided otherwise. On the coordinated date, OpenPrinting
group will push the fix into project and announce the vulnerability
on `oss-security@lists.openwall.com`. Announcement of such vulnerabilities won't require
immediate new release unless it is decided otherwise.
