<html>
<body>

Confirmation inscription for <?php
echo $_POST["FirstName"];

echo " ";
echo $_POST["LastName"];

echo ", your username will be: ";
echo $_POST["DisplayName"];
?>

<br>

<?php
echo "Confirm your email: ";
echo $_POST["Email"];
?>

</body>
</html>
