#!/usr/bin/env bash
# ─────────────────────────────────────────────────────────────────────────────
# test_all.sh — Runs every feature through its paces.
# Run from the project root:  bash tests/test_all.sh
# ─────────────────────────────────────────────────────────────────────────────

APP="./build/my_app"
XML="xml_inputs/test.xml"
PASS=0
FAIL=0

# Helper — prints a coloured PASS/FAIL line
check() {
    local description="$1"
    local condition="$2"
    if eval "$condition" > /dev/null 2>&1; then
        echo "  ✅  PASS — $description"
        ((PASS++))
    else
        echo "  ❌  FAIL — $description"
        ((FAIL++))
    fi
}

echo ""
echo "════════════════════════════════════════════════════"
echo "  XML Parser — Full Feature Test"
echo "════════════════════════════════════════════════════"

# ── 1. Binary exists ─────────────────────────────────────────────────────────
echo ""
echo "▶  Build & Binary"
check "Binary exists at build/my_app"          "[ -f $APP ]"

# ── 2. No arguments → usage message ─────────────────────────────────────────
echo ""
echo "▶  Argument Handling"
check "No args prints usage hint"              "$APP 2>&1 | grep -q 'Hey\|Usage\|usage\|how to'"
check "Missing --search tag shows error"       "$APP $XML --search 2>&1 | grep -q 'needs\|requires\|Oops'"
check "Nonexistent file shows friendly error"  "$APP no_such_file.xml 2>&1 | grep -q \"couldn't open\|Uh oh\|exist\""


# ── 3. Default parse + output file ───────────────────────────────────────────
echo ""
echo "▶  Parse & Format (default)"
check "Parses test.xml without error"          "$APP $XML"
check "Output file is created"                 "[ -f xml_outputs/test.xml.structured ]"
check "Output contains root tag 'Company'"     "grep -q '^\- Company' xml_outputs/test.xml.structured"
check "Output contains nested 'Headquarters'"  "grep -q 'Headquarters' xml_outputs/test.xml.structured"
check "Output contains text value for Street"  "grep -q 'Street: 100 Tech Park Drive' xml_outputs/test.xml.structured"
check "Output contains deep node 'Email'"      "grep -q 'Email' xml_outputs/test.xml.structured"

# ── 4. Sort ──────────────────────────────────────────────────────────────────
echo ""
echo "▶  Sort (--sort)"
SORTED=$($APP --sort $XML 2>&1)
check "--sort runs without error"              "$APP --sort $XML"
# After sorting, AuditTrail (A) should come before Headquarters (H) at root level
AUDIT_LINE=$(echo "$SORTED" | grep -n "AuditTrail" | head -1 | cut -d: -f1)
HQ_LINE=$(echo "$SORTED"   | grep -n "Headquarters" | head -1 | cut -d: -f1)
check "AuditTrail appears before Headquarters when sorted" "[ '$AUDIT_LINE' -lt '$HQ_LINE' ]"
# Within Financials: Expenses (E) before NetIncome (N) before Revenue (R)
EXP_LINE=$(echo "$SORTED" | grep -n "Expenses"   | head -1 | cut -d: -f1)
NET_LINE=$(echo "$SORTED" | grep -n "NetIncome"  | head -1 | cut -d: -f1)
REV_LINE=$(echo "$SORTED" | grep -n "Revenue"    | head -1 | cut -d: -f1)
check "Expenses before NetIncome when sorted"  "[ '$EXP_LINE' -lt '$NET_LINE' ]"
check "NetIncome before Revenue when sorted"   "[ '$NET_LINE' -lt '$REV_LINE' ]"

# ── 5. Search ────────────────────────────────────────────────────────────────
echo ""
echo "▶  Search (--search)"
check "Search for 'Employee' finds results"    "$APP --search Employee $XML | grep -q 'Found'"
check "Search finds exactly 5 Employees"       "$APP --search Employee $XML | grep -q 'Found 5'"
check "Search for 'Email' shows text values"   "$APP --search Email $XML | grep -q 'value :'"
check "Search for 'Email' finds 6 results"     "$APP --search Email $XML | grep -q 'Found 6'"
check "Search for 'Revenue' finds value 985"   "$APP --search Revenue $XML | grep -q '985'"
check "Search for 'Skill' finds 16 results"    "$APP --search Skill $XML | grep -q 'Found 16'"
check "Search path format uses ' > ' separator" "$APP --search Email $XML | grep -q ' > '"
check "Search for nonexistent tag says so"     "$APP --search Banana $XML | grep -q \"Couldn't find\|No elements\""
check "Search is case-sensitive (email≠Email)" "! $APP --search email $XML | grep -q 'Found [0-9]'"

# ── 6. Combined flags ────────────────────────────────────────────────────────
echo ""
echo "▶  Combined flags (--sort --search)"
check "--sort --search together works"         "$APP --sort --search Email $XML | grep -q 'Found 6'"
# When sorted, Headquarters (H) comes after Employees (E), so HQ email appears later
COMBINED=$($APP --sort --search Email $XML 2>&1)
HQ_IDX=$(echo "$COMBINED"  | grep -n "Headquarters" | head -1 | cut -d: -f1)
EMP_IDX=$(echo "$COMBINED" | grep -n "Employee"     | head -1 | cut -d: -f1)
check "In sorted+search, Employee email appears before Headquarters email" \
      "[ '$EMP_IDX' -lt '$HQ_IDX' ]"

# ── Summary ──────────────────────────────────────────────────────────────────
TOTAL=$((PASS + FAIL))
echo ""
echo "════════════════════════════════════════════════════"
printf "  Results:  %d / %d passed" "$PASS" "$TOTAL"
if [ "$FAIL" -eq 0 ]; then
    echo "  🎉 All good!"
else
    echo "  ⚠️  $FAIL test(s) failed — check output above"
fi
echo "════════════════════════════════════════════════════"
echo ""

# Exit with non-zero if anything failed
[ "$FAIL" -eq 0 ]
