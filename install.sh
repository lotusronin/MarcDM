#! /bin/sh

# By Marcus Godwin

check_makefile() {
    # Check for Makefile, if it doesn't exist call qmake
    if [ ! -f Makefile ]; then
        printf '%s\n' 'Generating Makefile'
        qmake -o Makefile project.pro
    fi
}

user_group_setup() {
    # Check for marcdm group and user, make them if necessary
    # users should not be able to login as the marcdm user
    if ! getent group marcdm > /dev/null; then
        printf '%s\n' 'You do not have marcdm group, making it now'
        sudo groupadd --system marcdm
    fi
    if ! getent passwd marcdm > /dev/null; then
        printf '%s\n' 'no passwd entry for marcdm, making it now' 
        sudo useradd -c "Marc Display Manager" --system -d /usr/bin/marcdm -s /usr/bin/nologin -g marcdm marcdm
        sudo passwd -l marcdm
    fi
}

move_text_files() {
    # Xsession script
    sudo cp -f ./scripts/Xsession /etc/marcdm/
    # config file
    if [ ! -f /etc/marcdm/marcdm.conf ]; then
        sudo cp ./marcdm.conf /etc/marcdm/
    fi
    # systemd service file
    sudo cp -f ./marcdm.service /usr/lib/systemd/system/
    # pam file
    sudo cp -f ./pam/marcdm /etc/pam.d/marcdm
}

printf '%s\n' 'Installing marcdm'

check_makefile
# Compile project
make

printf '%s\n' 'Post Installation Steps'

user_group_setup

# move files to the proper place
printf '%s\n' 'Moving files'
# binary file
sudo cp -f marcdm /usr/bin/
sudo cp -f marcdm-helper /usr/bin/
move_text_files

printf '%s\n' 'All done installing'
printf '%s\n' 'To enable marcdm as your display manager run: systemctl enable marcdm.service'
