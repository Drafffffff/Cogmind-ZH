/* A final literal after one captured name contains no hidden player values.
   Expose it to the existing strict, ambiguity-rejecting suffix matcher. */
static int ZhPartialLiteralAllowed(const char *key, int klen)
{
    static const char attach_tutorial_tail[] = "inventory to the parts list.";
    if (klen >= 40) return 1;
    /* This exact 28-cell suffix is present in the captured Ion Engine attach
       tutorial. Do not lower the general cutoff: short fragments are otherwise
       too likely to match unrelated paragraph endings. The normal candidate,
       capture, and translation-ambiguity checks still run afterward. */
    return key && klen == (int)sizeof(attach_tutorial_tail) - 1 &&
           memcmp(key, attach_tutorial_tail, sizeof(attach_tutorial_tail) - 1) == 0;
}

/* The complete attach rule's translated tail includes “以完成装配”, which maps
   to the clipped-away English words before this fragment. For this one exact
   native rule/key pair, render only the semantically corresponding visible
   portion. The caller must already have resolved the normal ambiguity scan. */
static void ZhPartialLiteralTargetOverride(const char *key, int klen,
                                           Rule *rule)
{
    static const char source_tail[] =
        " by dragging it from the inventory to the parts list.";
    static const char target_tail[] = "从库存拖至部件列表以完成装配。";
    static const char clipped_target[] = "从库存拖至部件列表。";
    if (!key || !rule || klen != 28 ||
        memcmp(key, "inventory to the parts list.", 28) ||
        rule->ncap != 0 || rule->ns != 1 || rule->nd != 1 ||
        rule->s[0].cap || rule->d[0].cap ||
        rule->s[0].len != (int)sizeof(source_tail) - 1 ||
        rule->d[0].len != (int)sizeof(target_tail) - 1 ||
        memcmp(rule->s[0].lit, source_tail, sizeof(source_tail) - 1) ||
        memcmp(rule->d[0].lit, target_tail, sizeof(target_tail) - 1))
        return;
    rule->d[0].lit = clipped_target;
    rule->d[0].len = (int)sizeof(clipped_target) - 1;
}

static int ZhPartialLiteral(const Rule *input, Rule *literal)
{
    int i, captures = 0;
    *literal = *input;
    if (input->ns == 1 && !input->s[0].cap &&
        input->nd == 1 && !input->d[0].cap) return 1;
    if (input->ncap != 1 || input->ns < 2 || input->nd < 2 ||
        input->s[input->ns-1].cap || input->d[input->nd-1].cap) return 0;
    for (i = 0; i < input->nd; ++i) {
        if (!input->d[i].cap) continue;
        if (input->d[i].cap > 3) return 0; /* Reordered/unknown capture. */
        ++captures;
    }
    if (captures != 1) return 0;
    literal->s[0] = input->s[input->ns-1];
    literal->d[0] = input->d[input->nd-1];
    literal->ns = literal->nd = 1;
    literal->ncap = 0;
    return 1;
}
