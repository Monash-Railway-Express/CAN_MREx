# **What you must do for every new Arduino library release**
---

## **1. Update `library.properties`**
You must update:

- **version=**  
  Use semantic versioning (e.g., `1.0.1`, `1.1.0`, `2.0.0`)

- **url=**  
  Should point to your GitHub repo (usually unchanged)

- **sentence**, **paragraph**, **category**, etc.  
  Only change if needed, not required for updates.

**Important:**  
The version in `library.properties` **must match** the GitHub release tag.

---

## **2. Create a new GitHub Release**
Arduino’s Library Manager bot only detects **GitHub Releases**, not commits.

Steps:

1. Go to **Releases** → **Draft a new release**
2. Create a tag like:  
   `v1.0.1`  
   (must match `version=1.0.1` in `library.properties`)
3. Add release notes (optional)
4. Publish

The Arduino bot scans hourly and updates automatically.

---

## **3. Ensure your repo structure is valid**
This must be correct **every time**, not just the first release:

```
your-library/
 ├── library.properties
 ├── src/
 │    ├── YourLibrary.cpp
 │    └── YourLibrary.h
 ├── examples/
 └── keywords.txt   (optional)
```

If you break the structure, the Library Manager will reject the update.

---

## **4. If your library is already in the Library Manager**
You don’t need to submit anything again.

Just:

- Update `library.properties`
- Publish a new GitHub release

The bot handles the rest.

---

## **5. If your library is NOT yet in the Library Manager**
You must submit it **once**:

- Open an issue in:  
  `arduino/library-registry`
- Use “Add a Library”
- Provide your GitHub repo URL

After approval, all future releases auto‑update.

---

# **Quick Release Checklist**

| Step | Required? | Notes |
|------|-----------|-------|
| Update `version=` in `library.properties` | ✔ | Must match Git tag |
| Commit the updated file | ✔ | Before creating release |
| Create GitHub Release with matching tag | ✔ | Bot only reads releases |
| Keep folder structure valid | ✔ | Otherwise update fails |
| Submit to Library Manager (first time only) | ✔ | Only once |

---
