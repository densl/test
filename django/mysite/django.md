
# check django version
	python -c "import django; print(django.get_version())"

##creating a project
	django-admin.py startproject mysite

##Database setup
	python manage.py migrate

##Run server
	python manage.py runserver

##Creating models
	python manage.py startapp polls

  vim polls/models.py
	class Question(models.Model):
          question_text = models.CharField(max_length=200)
          pub_date = models.DateTimeField('date published')

	class Choice(models.Model):
          question = models.ForeignKey(Question)
          choice_text = models.CharField(max_length=200)
          votes = models.IntegerField(default=0)


##Activating models
  vim mysite/settings.py
	INSTALLED_APPS = (... 'polls',)

#store changes to migration
	python manage.py makemigrations polls

#take migration names and returns their SQL
	python manage.py sqlmigrate polls 0001

#checks for any problems
	python manage.py check

#run migrate again to create those model tables
	python manage.py migrate

#use django api
	python manage.py shell


##Creating an admin user
	python manage.py createsuperuser


##Start the development server
	python manage.py runser


