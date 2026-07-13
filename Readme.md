# 👋 XML Parser

Hey there! Welcome to the XML Parser project. This tool lets you take any XML file, 
read through its structure, sort it alphabetically, or hunt down specific elements — 
all from the command line. It's simple, flexible, and easy to extend.

## What we're building towards

Here's the roadmap of features we're working on:

1. 📂 **File handling** — read XML files in, write structured output out
2. ✅ **Structure validation** — catch broken or malformed XML before it causes trouble
3. 🌲 **Data structure** — represent the XML as a clean in-memory tree
4. 🔄 **File ↔ data structure** — convert back and forth between file and tree
5. 🗜️ **Compression** — squeeze output down when you need it compact
6. 🧪 **Unit testing** — make sure nothing breaks as the project grows

---

## How the project is laid out

```
xml_parsing/
├── include/
│   ├── xml_parser.h       ← the heart of it all (always needed)
│   ├── xml_sorter.h       ← optional: want sorted output? this is it
│   └── xml_searcher.h     ← optional: want to find elements? this one's for you
├── src/
│   ├── xml_parser.cpp
│   ├── xml_sorter.cpp     ← remove this file to turn sorting off
│   └── xml_searcher.cpp   ← remove this file to turn searching off
├── samples/
│   └── main.cpp           ← where it all comes together
├── xml_inputs/            ← drop your XML files in here
├── xml_outputs/           ← structured output lands here automatically
└── CMakeLists.txt
```

> 💡 **Good to know:** The sort and search features are totally plug-and-play. 
> Delete their files and the core parser keeps working just fine — CMake figures it out on its own.

---

## Building the project

Just two commands and you're ready to go:

```bash
# Step 1: set up the build directory
cmake -S . -B build

# Step 2: compile everything
cmake --build build
```

When you run the first command, CMake will tell you which optional features it found:

```
-- xml_sorter module found — sorting feature enabled
-- xml_searcher module found — search feature enabled
```

No messages about a module? That just means you haven't added those files yet — nothing to worry about.

---

## How to use it

```
./build/my_app [--sort] [--search <tag>] <your_file.xml>
```

### Just parse it (the default)

Hand it an XML file and it'll print a nice structured tree, plus save a copy to `xml_outputs/`:

```bash
./build/my_app xml_inputs/test.xml
```

You'll get something like:
```
- Company
  - Headquarters
    - Address
      - Street: 100 Tech Park Drive
      - City: New York
  ...
```

---

### Sort the output (`--sort`)

Want everything in alphabetical order? Add `--sort` and every level of the tree gets sorted by tag name:

```bash
./build/my_app --sort xml_inputs/test.xml
```

---

### Search for a specific element (`--search <tag>`)

Looking for a particular tag? Use `--search` followed by the tag name you're after.
It'll show you every match, along with the full path to get there:

```bash
./build/my_app --search Employee xml_inputs/test.xml
./build/my_app --search Email xml_inputs/test.xml
```

Example output:
```
Found 5 element(s) matching <Employee>:
--------------------------------------------------
[1] Company > Employees > Employee
[2] Company > Employees > Employee
...
--------------------------------------------------
```

If the element has a text value, it shows that too:
```
Found 6 element(s) matching <Email>:
--------------------------------------------------
[1] Company > Employees > Employee > Contact > Email
     value : arjun.mehta@globaltech.com
...
--------------------------------------------------
```

And if it can't find anything:
```
No elements found with tag <Banana>.
```

---

### Mix and match flags

Flags work together! Sort first, then search the sorted result:

```bash
./build/my_app --sort --search Email xml_inputs/test.xml
```

---

## The optional modules explained

The sort and search features live in their own little worlds — completely separate from the core parser.
You can add or remove them without touching `xml_parser.h` or `xml_parser.cpp` at all.

| Feature | What you need | Enabled when... | CLI flag |
|---|---|---|---|
| Sort | `include/xml_sorter.h` + `src/xml_sorter.cpp` | both files are present | `--sort` |
| Search | `include/xml_searcher.h` + `src/xml_searcher.cpp` | both files are present | `--search <tag>` |

To disable a feature, just delete its files and re-run `cmake -S . -B build`.
If you accidentally pass `--sort` or `--search` without the module installed, 
the app will let you know with a friendly warning and carry on without it.