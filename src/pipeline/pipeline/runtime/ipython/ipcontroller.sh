export PYTHONPATH="/data/users/swinbank/app/lib/python2.5/site-packages:/app/usg/release/lib/python"
export PATH="/data/users/swinbank/app/bin:/usr/local/bin:/usr/bin:/bin:/usr/games"
export LD_LIBRARY_PATH="/data/users/swinbank/app/lib/"
CONTROLPATH=$1

mkdir -p $CONTROLPATH

case "$2" in
  start) 
         /sbin/start-stop-daemon --start -b -m --pidfile $CONTROLPATH/ipc.pid \
           --exec /data/users/swinbank/app/bin/ipcontroller -- -xy \
           --engine-furl-file=$CONTROLPATH/engine.furl \
           --task-furl-file=$CONTROLPATH/task.furl \
           --multiengine-furl-file=$CONTROLPATH/multiengine.furl
         ;;
  stop)
         /sbin/start-stop-daemon --stop --pidfile $CONTROLPATH/ipc.pid
         ;;
  *)
         echo "Usage: $0 <controlpath> {start|stop}" >&2
         exit 1
         ;;
esac

exit 0
