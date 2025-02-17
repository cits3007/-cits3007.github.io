---
title: |
  CVSS scores
header-includes: |
  ```{=html}
  <style>
  div#agreement img {
    border: solid 0.5pt hsl(0 0% 80% / 0.5);
  }
  </style>
  ```
---

## What is CVSS? Are there alternatives?

CVSS is a methodology for assessing vulnerability severity, but others
exist -- any individual or organisation is free to make estimates of
CVE severity using any scale they wish.
Red Hat, for instance, rates the
severity of security issues found in their products using
a four-point scale (Red Hat, 2022).

## Where can CVSS scores be obtained from?

Any person or company can publish their own CVSS score
(for instance, Oracle provides its own CVSS assessments
- <https://www.oracle.com/security-alerts/cvssscoringsystem.html>),
but one common source of CVSS scores is the National Vulnerability Database
(NVD),
a database of vulnerabilities which uses the CVE as a source and adds
additional information (MITRE Corporation, 2020).

The NVD publishes CVSS scores using both version 2 and version 3
of the CVSS standard. Security industry participants had made
many criticisms of version 2 of the CVSS (see, for instance,
Eiram & Martin, 2013), and version 3 was developed in response
to this feedback and released in June 2015. CVSS version 3 aims to
be less ambiguous and easier to apply than version 2, and to allow
more granular scoring.

## Why do different organisations publish different CVSS scores?

Different organisations may come up
with different estimates of severity when applying the CVSS.

For instance, consider CVE-2022-1271. AusCERT publishes a CVSS score
of 7.1 (see <https://auscert.org.au/bulletins/ESB-2022.2760>), but the
NVD website has the severity as being 8.8 (see
<https://nvd.nist.gov/vuln-metrics/cvss/v3-calculator?name=CVE-2022-1271>).

AusCERT are using Red Hat's assessment of the CVSS:

```markdown
Comment: CVSS (Max):  7.1 CVE-2022-1271 (CVSS:3.1/AV:N/AC:H/PR:L/UI:R/S:U/C:H/I:H/A:H)
         CVSS Source: Red Hat
         Calculator:  https://www.first.org/cvss/calculator/3.1#CVSS:3.1/AV:N/AC:H/PR:L/UI:R/S:U/C:H/I:H/A:H
```

The bit of text in parentheses after the CVE on the first line is called
a CVSS "vector", and is a compact listing of the metrics used to
calculate the CVSS score.
(The "calculator" link on the last line breaks down and explains
the components of this vector.)

If we visit the NVD, we'll see that NIST has
used a different vector:

```
AV:N/AC:L/PR:L/UI:N/S:U/C:H/I:H/A:H
```

This can
be interpreted by visiting
<https://www.first.org/cvss/calculator/3.1#CVSS:3.1/AV:N/AC:L/PR:L/UI:N/S:U/C:H/I:H/A:H>:

- "AV" means "Attack Vector", and "AV:N" means that
  the attacker only needs network access to exploit
  the vulnerability (as opposed to, for instance, physical access).
- "AC" means "Attack Complexity", and "AC:L" means the
  complexity of the attack is "Low" -- the vulnerability is relatively
  easy to exploit.
- "PR" means "Privileges Required", and "PR:L" means "Low":
  some low level of authorization is required in order to mount the attack.
- "UI" means "User Interaction", and "UI:N" means the
  vulnerability can be exploited without an end user having
  to take any sort of action. (Compare this with, say, a phishing
  email, where an end user *does*
  have to take some sort of action for the attack to be successful --
  they have to click on a link in the phishing email).
- "S" means "Scope", and "S:U" ("Scope: Unchanged") basically means that
  exploiting this vulnerability does not give an attacker access
  to other components.
- "C" means "Confidentiality", and "C:H" ("Confidentiality: High") means
  there is a *total* loss of confidentiality -- typically meaning that
  all information resources within the vulnerable component are now
  accessible to the attacker.
- "I" means "Integrity", and "I:H" ("Integrity: None") means there is
  a complete loss of intergrity.
- Finally, "A" means "Availability", and "A:H" ("Availability: High")
  means there is a "total loss of availability, resulting in the attacker
  being able to fully deny access to resources in the impacted component"
  (FIRST.Org, Inc., 2019).

Notice that NIST and Red Hat come to different conclusions about
some factors.
For instance, Attack Complexity (AC) is rated by the NVD
as "Low", meaning easier for an attacker to exploit, whereas Red Hat
rate it as "High".

Notice also that on the calculator page, the vector string only
fills in boxes in the section labelled "base" score. There
are two other sections for "Temporal Score", and "Environmental Score",
but the boxes here are all set to "Not Defined (X)".

## What are the "Temporal" and "Environmental" scores?

Often, the CVSSv3 score an organisation publishes
is just a "Base" score.
The metrics going into the "Base" score assess how difficult it is for an attacker
to exploit a vulnerability.

However, a fuller CVSS score could also
assess to what degree exploits or fixes for the vulnerability
are available (these fall into what are called the "Temporal"
metrics, as they can change with time),
and what security goals, requirements and controls
exist for a particular user's environment (these fall into
what are called the "Environmental" metrics).

The idea is that published "Base" scores will be supplemented
with information about prevalence and fixes
(as these change), and that
a consumer of CVSS scores should supplement them
with "Environmental" information relevant to that consumer's organization.

So, for instance: if some particular confidential information resource
is particularly critical to an organisation that uses
CVSS scores, or if the chance of a vulnerability
being exploited is lower due to particular defences that have
been put in place, then those factors would go into the
calculation of the "Environmental" metrics.


## Are the NVD scores the most reliable?

Not necessarily.
NIST have been known to [make mistakes][nist-mistakes] in their assessments.
It's arguable
that the publishers of a particular software package (Microsoft, say) are better positioned than
NIST to understand how severe a vulnerability is and how easy it is to exploit. But this would vary from
organisation to organisation – if a particular publisher has a terrible security track record, you
might prefer NIST's rating (or ideally, reconsider using their products
at all).

An interesting report on applying
CVSS was published earlier this year by a security tools vendor, JFrog (report linked from
[this article][art]). Their recommendations included:

- If the organisation that reported a vulnerability gives a different CVSS
  to the NVD, then *in general* the reporting organisation's score is
  probably the result of a more in-depth analysis than NIST's
- If the organisation publishing your OS/distro (e.g. Ubuntu, Red Hat) has provided a CVSS score
  that differs from NIST's, then it probably includes factors specific
  to your distro and is more reliable.

[art]: https://portswigger.net/daily-swig/cvss-system-criticized-for-failure-to-address-real-world-impact
[nist-mistakes]: https://vulncheck.com/blog/cvss-accuracy-issues


## References

- Eiram, Carsten, and Brian Martin. "The CVSSv2 Shortcomings,
  Faults, and Failures Formulation." Open letter, 2013. Accessed August 1, 2022.
  <https://www.riskbasedsecurity.com/reports/CVSS-ShortcomingsFaultsandFailures.pdf>.

- FIRST.Org, Inc. "CVSS v3.1 Specification Document." FIRST.Org, Inc., June 2019. Accessed August 1, 2022.
  <https://www.first.org/cvss/v3.1/specification-document>.

- MITRE Corporation. "CVE and NVD Relationship." *CVE*. Last modified
  December 11, 2020. Accessed August 1, 2022.
  <https://cve.mitre.org/about/cve_and_nvd_relationship.html>.

- Red Hat. "Severity Ratings." *Red Hat Customer Portal*. Last modified
  September 27, 2022. Accessed August 1, 2022.
  <https://access.redhat.com/security/updates/classification>.



<!-- vim: syntax=markdown tw=72 smartindent :
-->
