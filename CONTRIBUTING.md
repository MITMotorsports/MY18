# Contributing to the MY18 repository

Hey, thanks for taking the time to read this before contributing!

Before we get started, please be aware that **you will _not_ be able to push code _directly_ to the `master` branch**.
This is to ensure that the master branch contains commits that represent _only_ accepted working states of the car.
Thus, the only way for `master` to change is via merge commits, or emergency administrator commits.

As such, you must write your code in separate branches and then integrate your changes via a _pull request_.

## Creating Development Branches

It is very important for you to be aware of the branch you are working on, when writing code.
After setting up this repository on your machine, make sure that you create, if needed, and work on the **appropriate branch**.

### Branch Naming

Your branches should adhere to the following scheme, in order to reduce confusion.

`<project name>-<branch description>`

where `<project name>` describes your project (ex. `bms`, `vcu`, `dash`)
and `<branch description>` is a short but meaningful description of what is being worked on in the branch.

Please try your best to **adhere to a single `<project name>`** when creating multiple branches.

**It is acceptable to have and work on only a single branch, named after your project.**    
There is no pressure to partition multiple types of changes over multiple branches, as long as you believe it will not be confusing to review and eventually merge.

#### Do

- `logger`
- `vcu-limp-mode`
- `bms-error-checks`
- `dash-fast-pages`

#### Don't

- `vcu-add-limp-mode-feature`
- `dashboard-button-support` - stick to a single `<project name>`

### Branch Source

Your new branch **should** originate from the latest commit in `master`.
However, this is not a strict rule; if you feel it would ease development, you could branch off any commit in a dev branch.    
Ideally, you would **pull** your source branch's latest changes **before** branching.

The commands you could run to achieve this are:

```
git checkout master
git pull
git checkout -b <name>
```

where `<name>` is your new branch's name.
