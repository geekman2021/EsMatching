<div class="container mt-4">
    <table class="table table-bordered">
        <thead style="text-align: center;">
            <tr >
                <th style="display: none;">Id </th>
                <th>Code Droit</th>
                <th>Permission</th>
                <th>Action</th>
            </tr>
        </thead>
        <tbody style="text-align: center;">
            <?php foreach ($data as $droit) { ?>
                <tr>
                    <td style="display: none;"><?php echo $droit->id; ?></td>
                    <td><?php echo $droit->code_droit; ?></td>
                    <td><?php echo $droit->permission; ?></td>
                    <td style="text-align: center ;">
                        <form action="" method="DELETE">
                            <a class="btn btn-info" href="#" onclick="modifier('<?php echo $droit->id; ?>', '<?php echo $droit->code_droit; ?>', '<?php echo $droit->permission; ?>')">Modifier</a>
                            <button class="btn btn-danger" onclick="supprimer('<?php echo $droit->id; ?>')">Supprimer</button>
                        </form>
                    </td>
                </tr>
            <?php }; ?>
        </tbody>
    </table>
</div>

<script>
    function modifier(id, code_droit, permission) {
        document.getElementById("id").value= id;
        document.getElementById("code_droit").value= code_droit;
        document.getElementById("permission").value= permission;
        document.getElementById("code_droit").disabled= true;

    }

    function supprimer(id) {
        var confirm= window.confirm("voulez-vous vraiment supprimer");

        if(confirm) {
            $.ajax({
                url: "<?php echo site_url("droits/remove") ?>",
                type: "POST",
                data: {
                    "id": id
                },
                success: function (response) {
                    window.location.reload();
                },
                error: function (error) {
                    alert(error);
                }
            });

        }
    }

    $("#addForm").submit(function(e) {
        e.preventDefault();
        var id= $("#id").val();
        var code_droit= $("#code_droit").val();
        var permission= $("#permission").val();

        console.log(id, code_droit, permission);

        if(id) {
            $.ajax({
                url: "<?php echo site_url("droits/edit") ?>",
                type: "PUT", 
                data: JSON.stringify({
                    id: id,
                    code_droit: code_droit,
                    permission: permission
                }),
                success: function (response) {
                    window.location.reload();
                },
                error: function (error) {
                    alert(error);
                }
            });
        } else {
            $.ajax({
                url: "<?php echo site_url("droits/create") ?>",
                type: "POST",
                data: {
                    "code_droit": code_droit,
                    "permission": permission
                },
                success: function (response) {
                    window.location.reload();
                },
                error: function (error) {
                    alert(error);
                }
            }); 
        }
    })



</script>